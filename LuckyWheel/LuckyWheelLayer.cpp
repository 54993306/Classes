#include "LuckyWheelLayer.h"
#include "common/CShowToBuyResource.h"
#include "common/CheckMoney.h"
#include "common/CGameSound.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/UICloneMgr.h"
#include "tools/ShowTexttip.h"
#include "GMessage.h"
#include "sign/PopItem.h"

CLuckyWheelLayer::CLuckyWheelLayer(void):
	m_monsterInfo(nullptr),
	m_itemInfo(nullptr),
	m_ui(nullptr),
	m_item(nullptr),
	m_pointer(nullptr),
	m_prizeItemTag(nullptr),
	m_onceBtn(nullptr),
	m_tenBtn(nullptr),
	m_closeBtn(nullptr),
	m_iRotateAngle(0),
	m_pItemListNum(0),
	m_lotteryType(0),
	m_recordTime(0),
	m_onceNeedGold(0),
	m_tenNeedGold(0),
	m_minPos(0),
	m_maxPos(0),
	m_showNoticeLabNum(0),
	m_isRotateAction(false),
	m_lightNum(0),
	m_iPrizeAngle(0),
	m_changelight(0),
	m_randomAng(0),
	m_startRotateTime(0),
	m_isSlow(false),
	m_isUpdateItemList(true)
{

}

CLuckyWheelLayer::~CLuckyWheelLayer(void)
{
}

bool CLuckyWheelLayer::init()
{
	if(BaseLayer::init())
	{
		CCLOG("init LuckyWheelLayer");
		MaskLayer* pMaskLayer = MaskLayer::create("CLuckyWheelLayerMask");
		pMaskLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
		LayerManager::instance()->push(pMaskLayer);

		m_ui = LoadComponent("LuckyWheel.xaml");
		m_ui->setPosition(VCENTER);
		this->setOpacity(180);
		this->addChild(m_ui);

		return true;
	}

	return false;
}

void CLuckyWheelLayer::onEnter()
{
	BaseLayer::onEnter();

	//人物动画
	SkeletonAnimation* ani = SkeletonAnimation::createWithFile("luckyWheel/qiuqiannv.json","luckyWheel/qiuqiannv.atlas",1);
	ani->setPosition(ccp(200,10));
	ani->setAnimation(0,"stand",true);
	m_ui->addChild(ani,-10);

	m_closeBtn = (CButton*)m_ui->findWidgetById("closeBtn");
	m_closeBtn->setPosition(VLEFT+50 ,VTOP-50);
	m_closeBtn->getSelectedImage()->setScale(1.1f);
	m_closeBtn->setOnClickListener(this,ccw_click_selector(CLuckyWheelLayer::onClose));

	m_onceBtn = (CButton*)m_ui->findWidgetById("onceBtn");
	m_onceBtn->getSelectedImage()->setScale(1.1f);
	m_onceBtn->setOnClickListener(this, ccw_click_selector(CLuckyWheelLayer::onWheelBtnOnceClick));

	m_tenBtn = (CButton*)m_ui->findWidgetById("tenBtn");
	m_tenBtn->getSelectedImage()->setScale(1.1f);
	m_tenBtn->setOnClickListener(this, ccw_click_selector(CLuckyWheelLayer::onWheelBtnTenClick));

	m_pointer = (CImageView*)m_ui->findWidgetById("pointer");
	m_pointer->setAnchorPoint(ccp(0.5f,0.5f));

	m_prizeItemTag = (CImageView*)m_ui->findWidgetById("prize");
	m_prizeItemTag->setAnchorPoint(ccp(0.5,0));
	m_prizeItemTag->setPosition(m_pointer->getPosition());
	m_prizeItemTag->setVisible(false);

	CButton* updateItemList = (CButton* )m_ui->findWidgetById("updateItemList");
	updateItemList->setOnClickListener(this,ccw_click_selector(CLuckyWheelLayer::onUpdateItemListClick));

	GetTcpNet->registerMsgHandler(LuckyWheelMsg,this,CMsgHandler_selector(CLuckyWheelLayer::ProcessMsg));
	GetTcpNet->registerMsgHandler(DrawRequestMsg,this,CMsgHandler_selector(CLuckyWheelLayer::ProcessMsg));
	srand(time(NULL));

	initNoticeLabelShow();
	this->schedule(schedule_selector(CLuckyWheelLayer::updateNoticeLabelShowSchedule),1.5f,-1,1);
	this->schedule(schedule_selector(CLuckyWheelLayer::updateLightActionEffect),0.05f,-1,0);
}

void CLuckyWheelLayer::ProcessMsg(int type, google::protobuf::Message* msg)
{
	CCLOG("processNetmsg");
	if(!isVisible())
	{
		this->setVisible(true);
	}
	//接收网络数据
	if(type == LuckyWheelMsg)//转盘信息
	{
		CCLOG("LuckyWheelMsg");
		LuckyWheel* data = (LuckyWheel* )msg;
		m_prizeList.clear();
		m_noticeList.clear();
		m_onceNeedGold = -1;

		m_onceNeedGold = data->price_1();
		m_tenNeedGold = data->price_10();
		m_pItemListNum = data->prize_list_size();

		for (int i =0; i < data->prize_list_size();i++)
		{
			CPrize prize;
			prize.read(data->prize_list(i));
			m_prizeList.push_back(prize);
		}
		
		for (int i = 0; i< data->notices_size() ;i++)
		{
			CNotice notice;
			notice.read(data->notices(i));
			m_noticeList.push_back(notice);
		}

		//更新公告显示
		updateNoticeLabelShow();
		//更新奖励列表
		updateLotteryItem();

	}else if (type == DrawRequestMsg)//抽奖结果
	{
		CCLOG("DrawRequestMsg");
		DrawResponse* lotter = (DrawResponse* )msg;

		m_getPrizeList.clear();
		for (int i =0 ;i<10 ;i++)
		{
			m_prizeIndex[i] = -1;
		}

		switch (lotter->result())
		{
		case 1://成功
			{
				if (m_lotteryType == onceLottery)
				{
					UserData* userData = DataCenter::sharedData()->getUser()->getUserData();
					userData->setRoleGold(userData->getRoleGold() - m_onceNeedGold);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					for (int i = 1; i <= m_prizeList.size(); i++)
					{
						if (lotter->prize_index(0) == i)
						{
							m_getPrizeList.push_back(m_prizeList[i-1]);
						}
					}
					m_prizeIndex[0] = lotter->prize_index(0);
					CCLOG("prize_index_is_%d",m_prizeIndex[0]);
					showOnceRotate();
					updateLotteryItem();

				}else if(m_lotteryType == tenLottery)
				{
					UserData* userData = DataCenter::sharedData()->getUser()->getUserData();
					userData->setRoleGold(userData->getRoleGold() - m_tenNeedGold);
					CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);

					for (int i = 0; i < lotter->prize_index_size();i++)
					{
						int index = lotter->prize_index(i);
						CCLOG("prize_index_is_%d",index);
						m_prizeIndex[i] = index;

						for (int i = 1; i <= m_prizeList.size(); i++)
						{
							if (index == i)
							{
								m_getPrizeList.push_back(m_prizeList.at(i-1));
								//CCLOG();
							}
						}
					}

					showTenRotate();
				}

				PlayEffectSound(SFX_407);
			}
			break;
		case 2://勾玉不足
			{
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceAction);
			}
			break;
		case 3://数据错误
			{
				ShowPopTextTip(GETLANGSTR(170));
			}
			break;
		default:
			break;
		}
	}
}

void CLuckyWheelLayer::onPress(CCObject* obj, CTouchPressState state)
{
	CImageView* image = (CImageView* )obj;
	CPrize* prize = (CPrize* )image->getUserData();
	int iType = prize->type;
	
	if (m_monsterInfo)
		m_monsterInfo->hide();
	if (m_itemInfo)
		m_itemInfo->hide();


	//弹框
	switch (iType)
	{
	case 3:
	case 4:
	case 5:
	case 1:
		{
			if (m_itemInfo == nullptr)
			{
				m_itemInfo = CItemInfo::create();
				this->addChild(m_itemInfo);
			}
			switch (state)
			{
			case CTouchPressOn:
				{
					//设置物品弹框信息
					PlayEffectSound(SFX_429);
					CPrize* prize = (CPrize*)image->getUserData();
					m_itemInfo->setInfo(prize);
					m_itemInfo->bindPos(image);
					m_itemInfo->changeForSign(prize);
					m_itemInfo->show();

					CCLOG("itemInfo_show");
				}
				break;
			case CTouchPressOff:
				{
					m_itemInfo->hide();
					CCLOG("iteminfo_hide");
				}
				break;
			default:
				break;
			}
		}
		break;
	case 2:
		{
			if (m_monsterInfo == nullptr)
			{
				m_monsterInfo = CMonsterInfo::create();
				this->addChild(m_monsterInfo);
			}
			switch (state)
			{
			case CTouchPressOn:
				{
					//设置人物弹框信息
					PlayEffectSound(SFX_429);

					CPrize* prize = (CPrize*)image->getUserData();
					CHero hero;
					hero.heroid = prize->id;
					hero.thumb = prize->thumb;
					hero.quality = prize->quality;
					hero.iColor = prize->color;
					hero.level = 1;
					m_monsterInfo->setHero(&hero);
					m_monsterInfo->bindPos(image);
					m_monsterInfo->changeForSign(&hero);
					m_monsterInfo->show();

					CCLOG("monsterInfo_show");
				}
				break;
			case CTouchPressOff:
				{
					m_monsterInfo->hide();
					CCLOG("monsterInfo_hide");
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}
void CLuckyWheelLayer::onClose(CCObject* obj)
{
	this->setVisible(false);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}
void CLuckyWheelLayer::onUpdateItemListClick(CCObject* pSender)
{
	m_isUpdateItemList = true;
	updateLotteryItem();
}

void CLuckyWheelLayer::onWheelBtnOnceClick(CCObject* obj)
{
	CCLOG("once");
	//货币不足
	if(!CheckGold(m_onceNeedGold))
	{
		CShowToBuyResource* pShow = CShowToBuyResource::create();
		pShow->showToBuyResourceByType(ShowBuyResourceGold);
		return;
	}else
	{
		//if (m_onceNeedGold == 0)
		//{
		//	m_onceNeedGold = 100;
		//}
		m_lotteryType = onceLottery;
		//发送请求
		DrawRequest* req = new DrawRequest;
		req->set_draw_type(m_lotteryType);
		GetTcpNet->sendData(req,DrawRequestMsg);
		delete req;

		m_onceBtn->setEnabled(false);
		m_tenBtn->setEnabled(false);
		m_closeBtn->setEnabled(false);
	}

}
void CLuckyWheelLayer::onWheelBtnTenClick(CCObject* obj)
{
	CCLOG("ten");
	//获取泰国语言设置十连抽确认弹框
	CCString* str = CCString::createWithFormat(GETLANGSTR(1028),m_tenNeedGold);
	ShowConfirmTextTip(str->getCString(), this,ccw_click_selector(CLuckyWheelLayer::isOKTenBtn));
}
void CLuckyWheelLayer::isOKTenBtn(CCObject* pSendr)
{
	CButton* btn = (CButton* )pSendr;
	if (btn->getTag() == PopTipConfirm)
	{
		//货币不足
		if(!CheckGold(m_tenNeedGold))
		{
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
			return;
		}else
		{
			m_lotteryType = tenLottery;
			//发送请求
			DrawRequest* req = new DrawRequest;
			req->set_draw_type(m_lotteryType);
			GetTcpNet->sendData(req,DrawRequestMsg);
			delete req;

			m_onceBtn->setEnabled(false);
			m_tenBtn->setEnabled(false);
			m_closeBtn->setEnabled(false);
		}
	}
	((CPopTip* )btn->getParent()->getParent())->onClose(nullptr);
}
void CLuckyWheelLayer::followAngle()
{
	//光标跟随指针的位置变动

	int pointerAngle = m_pointer->getRotation();
	pointerAngle %= 360;

	if (pointerAngle >=0 &&pointerAngle<18 || pointerAngle>=342 &&pointerAngle<=360)
		m_iPrizeAngle = 0;
	else if(pointerAngle>=18 &&pointerAngle<54)
		m_iPrizeAngle = 36;
	else if(pointerAngle>=54 &&pointerAngle<90)
		m_iPrizeAngle = 72;
	else if(pointerAngle>=90 &&pointerAngle<126)
		m_iPrizeAngle = 108;
	else if(pointerAngle>=126 &&pointerAngle<162)
		m_iPrizeAngle = 144;
	else if(pointerAngle>=162 &&pointerAngle<198)
		m_iPrizeAngle = 180;
	else if(pointerAngle>=198 &&pointerAngle<234)
		m_iPrizeAngle = 216;
	else if(pointerAngle>=234 &&pointerAngle<270)
		m_iPrizeAngle = 252;
	else if(pointerAngle>=270 &&pointerAngle<306)
		m_iPrizeAngle = 288;
	else if(pointerAngle>=306 &&pointerAngle<342)
		m_iPrizeAngle = 324;
	else
	{
		m_iPrizeAngle = 0;
		CCLOG("m_iRotateAngle Info error");
	}
}

void CLuckyWheelLayer::showOnceRotate()
{
	//显示一次抽奖动画
	CCLOG("showOnceRotate");

	m_isRotateAction = true;
	m_startRotateTime = 0;
	m_isSlow = false;

	int recordAngle = m_iRotateAngle + m_randomAng;
	m_randomAng = -15 + rand()%35;

	for (int i = 0;i< m_pItemListNum;i++)
	{
		if (m_prizeIndex[0] == m_itemRandomIndex[i])
		{
			m_iRotateAngle = 36* (i);
		}
	}
	
	m_pointer->runAction(CCSequence::create(
		CCEaseExponentialIn::create(CCRotateBy::create(2.0f,360)),
		CCRotateBy::create(4,360* 20+ 360- recordAngle),
		CCEaseExponentialOut::create(CCRotateBy::create(10,360 * 5 + m_iRotateAngle + m_randomAng)),
		CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForActionEnd)),
		nullptr));
}
void CLuckyWheelLayer::showTenRotate()
{
	//显示十次旋转动画
	m_isRotateAction = true;

	int recordAngle = m_iRotateAngle + m_randomAng;
	m_randomAng = -15 + rand()%35;
	for (int i =0; i<m_pItemListNum;i++)
	{
		if (m_prizeIndex[m_recordTime] == m_itemRandomIndex[i])
		{
			m_iRotateAngle = 36* (i);
		}
	}

	++m_recordTime;
	m_pointer->runAction(CCSequence::create(
		CCEaseExponentialOut::create(CCRotateBy::create(1,360*2- recordAngle +m_iRotateAngle +m_randomAng)),
		CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForTenActionGoing)),
		//CCDelayTime::create(1.5f),
		nullptr)); 
}
void CLuckyWheelLayer::callBackForTenActionGoing()
{
	//第一到第九次旋转回调函数

	//相同的tag设置两次会覆盖掉上一次的
	//CImageView *image = UICloneMgr::cloneImageView(m_prizeItemTag);
	//image->setVisible(true);
	//image->setRotation(m_iRotateAngle);
	//m_ui->addChild(image,-1,99);
	//CCLOG("%d",image->getTag());

	m_prizeItemTag->setVisible(true);
	m_prizeItemTag->setRotation(m_iRotateAngle);
	m_prizeItemTag->runAction(CCSequence::create(
		CCBlink::create(0.5,3),
		CCHide::create(),
		//CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForShowGainPrize)),
		nullptr));

	for (int i = 0; i < m_pItemListNum; i++)
	{
		if (m_prizeIndex[m_recordTime -1] == m_itemRandomIndex[i])
		{
			CCString* str = CCString::createWithFormat("prize_%d",i+1);
			CImageView* showPrize = (CImageView* )m_ui->findWidgetById(str->getCString());
			showPrize->runAction(CCSequence::create(
				CCHide::create(),
				CCDelayTime::create(0.5f),
				CCShow::create(),
				nullptr));
			break;
		}
	}
	int recordAngle = m_iRotateAngle + m_randomAng;
	m_randomAng = -15 + rand()%35;
	for (int i =0; i<m_pItemListNum;i++)
	{
		if (m_prizeIndex[m_recordTime] == m_itemRandomIndex[i])
		{
			m_iRotateAngle = 36* (i);
			break;
		}
	}
	++m_recordTime;
	if (m_recordTime < 10)
	{
		m_pointer->runAction(CCSequence::create(
			CCEaseExponentialOut::create(CCRotateBy::create(1,360*2- recordAngle +m_iRotateAngle + m_randomAng)),
			CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForTenActionGoing)),
			nullptr)); 
	}else if (m_recordTime >= 10)
	{
		m_pointer->runAction(CCSequence::create(
			CCEaseExponentialOut::create(CCRotateBy::create(1,360*2- recordAngle +m_iRotateAngle + m_randomAng)),
			CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForActionEnd)),
			nullptr));
	}
}
void CLuckyWheelLayer::callBackForActionEnd()
{
	//显示黄标闪烁
	CCLOG("callBackForActionEnd");

	m_prizeItemTag->setRotation(m_iRotateAngle);
	m_prizeItemTag->setVisible(true);
	if (m_lotteryType == onceLottery)
	{
		m_isRotateAction = false;
		initUILightOffEffect();

		m_prizeItemTag->runAction(CCSequence::create(
			CCBlink::create(2,5),
			CCHide::create(),
			CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForShowGainPrize)),
			nullptr));
	}
	else if (m_lotteryType == tenLottery)
	{
		m_isRotateAction = false;
		initUILightOffEffect();

		m_prizeItemTag->runAction(CCSequence::create(
			CCBlink::create(0.5,3),
			CCHide::create(),
			CCDelayTime::create(0.5f),
			CCCallFunc::create(this,callfunc_selector(CLuckyWheelLayer::callBackForShowGainPrize)),
			nullptr));

		for (int i = 0; i < m_pItemListNum; i++)
		{
			if (m_prizeIndex[m_recordTime -1] == m_itemRandomIndex[i])
			{
				CCString* str = CCString::createWithFormat("prize_%d",i+1);
				CImageView* showPrize = (CImageView* )m_ui->findWidgetById(str->getCString());
				showPrize->runAction(CCSequence::create(
					CCHide::create(),
					CCDelayTime::create(0.5f),
					CCShow::create(),
					nullptr));
				break;
			}
		}
	}
}
void CLuckyWheelLayer::callBackForShowGainPrize()
{
	//显示获得奖励弹框
	if (m_lotteryType == onceLottery)
	{
		CPrize& prize = m_prizeList[m_prizeIndex[0] -1];

		CPopItem* popItem = CPopItem::create();
		popItem->signPrize(&prize);
		popItem->setDescVisiable(true);
		LayerManager::instance()->push(popItem);

		m_onceBtn->setEnabled(true);
		m_tenBtn->setEnabled(true);
		m_closeBtn->setEnabled(true);
		//m_pointer->setRotation(0);
		//m_iRotateAngle = 0;

		updateUserData();
	}
	else if (m_lotteryType == tenLottery)
	{
		if (m_recordTime == 10)
		{
			CGetPrizeRes prizes;
			//int coin = 0;
			//int gold = 0;
			for (int i = 0;i< m_getPrizeList.size();++i)
			{
				//合并获得资源
				//if (m_getPrizeList.at(i).type == 3)
				//{
				//	++coin;
				//	if (coin <= 1)
				//		prizes.prizeList.push_back(m_getPrizeList.at(i));
				//	else
				//	{
				//		for (int coinIndex = 0 ;coinIndex< prizes.prizeList.size(); coinIndex++)
				//		{
				//			if (prizes.prizeList.at(coinIndex).type == 3)
				//				prizes.prizeList.at(coinIndex).num += m_getPrizeList.at(i).num;
				//		}
				//	}
				//}
				//else if (m_getPrizeList.at(i).type == 4)
				//{
				//	++gold;
				//	if (gold <= 1)
				//		prizes.prizeList.push_back(m_getPrizeList.at(i));
				//	else
				//	{
				//		for (int goldIndex = 0 ;goldIndex< prizes.prizeList.size(); goldIndex++)
				//		{
				//			if (prizes.prizeList.at(goldIndex).type == 4 )
				//				prizes.prizeList.at(goldIndex).num += m_getPrizeList.at(i).num;
				//		}
				//	}
				//}
				//else
					prizes.prizeList.push_back(m_getPrizeList.at(i));
			}
			CPopItem* popItem = CPopItem::create();
			LayerManager::instance()->push(popItem);
			popItem->popPrizeRes(&prizes);

			m_prizeItemTag->setVisible(false);
			for (int i = 1; i <= m_pItemListNum; i++)
			{
				CCString* str = CCString::createWithFormat("prize_%d",i);
				CImageView* showPrize = (CImageView* )m_ui->findWidgetById(str->getCString());
				showPrize->setVisible(false);
			}

			m_onceBtn->setEnabled(true);
			m_tenBtn->setEnabled(true);
			m_closeBtn->setEnabled(true);
			//m_pointer->setRotation(0);
			//m_iRotateAngle = 0;
			m_recordTime = 0;

			updateUserData();
		}
		//发送更新数据
		GetTcpNet->sendDataType(LuckyWheelMsg);//,true
		GetTcpNet->registerMsgHandler(LuckyWheelMsg,this,CMsgHandler_selector(CLuckyWheelLayer::ProcessMsg));
	}
}

void CLuckyWheelLayer::initUILightOffEffect()
{
	for (int i = 1; i <= 10; i++)
	{
		CCString* str_off= CCString::createWithFormat("light_%d_Copy",i);
		CImageView* light_off = (CImageView* )m_ui->findWidgetById(str_off->getCString());

		if (i%2 == 0)
			light_off->setVisible(true);
		else
			light_off->setVisible(false);
	}

}
void CLuckyWheelLayer::updateLightActionEffect(float delta)
{
	//调度器_UI动画
	++m_changelight;
	CImageView* maskWheel = (CImageView* )m_ui->findWidgetById("maskWheel");
	CImageView* maskPoint = (CImageView* )m_ui->findWidgetById("mask");
	CCTexture2D* spTextture1 = CCTextureCache::sharedTextureCache()->addImage("luckyWheel/light_1.png");
	CCTexture2D* spTextture2 = CCTextureCache::sharedTextureCache()->addImage("luckyWheel/light_2.png");
	CCTexture2D* spTextture3 = CCTextureCache::sharedTextureCache()->addImage("luckyWheel/light_3.png");

	if (m_isRotateAction)
	{
		//抽奖一次动画实现
		if (m_lotteryType == onceLottery)
		{
			++m_startRotateTime;
			if (m_startRotateTime >= 30 )
			{
				if (!m_isSlow)
				{
					m_prizeItemTag->setVisible(true);
					m_iPrizeAngle += 36;
					m_prizeItemTag->setRotation(m_iPrizeAngle);
					if (m_startRotateTime >= 100 && ((int )(m_prizeItemTag->getRotation()))%360 - 15 <= ((int )(m_pointer->getRotation()))%360 && 
						((int )(m_prizeItemTag->getRotation()))%360 + 15 >= ((int )(m_pointer->getRotation()))%360)
					{
						m_isSlow = true;
					}
				}
				else
				{
					followAngle();
					m_prizeItemTag->setVisible(true);
					m_prizeItemTag->setRotation(m_iPrizeAngle);
				}
			}else
			{
				followAngle();
				m_prizeItemTag->setVisible(true);
				m_prizeItemTag->setRotation(m_iPrizeAngle);
			}
		}
		//抽奖灯光闪烁效果
		for (int i = 1; i<= 10; i++ )
		{
			CCString* str = CCString::createWithFormat("light_%d",i);
			CImageView* light_on = (CImageView* )m_ui->findWidgetById(str->getCString());

			CCString* str_off= CCString::createWithFormat("light_%d_Copy",i);
			CImageView* light_off = (CImageView* )m_ui->findWidgetById(str_off->getCString());
			light_off->setVisible(false);
			light_on->setVisible(true);

			if(light_on->getTexture() == spTextture1)
				light_on->setTexture(spTextture2);
			else if(light_on->getTexture() == spTextture2)
				light_on->setTexture(spTextture3);
			else if(light_on->getTexture() == spTextture3)
				light_on->setTexture(spTextture1);
		}
		maskWheel->setRotation(0);
		if (maskWheel->isVisible())
			maskWheel->setVisible(false);
		else
			maskWheel->setVisible(true);

		if (maskPoint->isVisible())
			maskPoint->setVisible(false);
		else
			maskPoint->setVisible(true);
	}else
	{
		//m_prizeItemTag->setVisible(false);
		
		//不抽奖时特效
		if (m_changelight >= 10)
		{
			m_changelight = 0;
			for (int i = 1 ;i<= 10;i++)
			{
				CCString* str_on = CCString::createWithFormat("light_%d",i);
				CImageView* light_on = (CImageView* )m_ui->findWidgetById(str_on->getCString());
				CCString* str_off= CCString::createWithFormat("light_%d_Copy",i);
				CImageView* light_off = (CImageView* )m_ui->findWidgetById(str_off->getCString());

				if (light_off->isVisible())
				{
					light_on->setVisible(true);
					light_off->setVisible(false);
				}else
				{
					light_on->setVisible(false);
					light_off->setVisible(true);
				}
			}
			if (maskWheel->isVisible())
			{
				maskWheel->setVisible(false);
				maskWheel->setRotation(0);
			}else
			{
				maskWheel->setVisible(true);
				maskWheel->setRotation(90);
			}
			if (maskPoint->isVisible())
				maskPoint->setVisible(false);
			else
				maskPoint->setVisible(true);
		}

	}

}

void CLuckyWheelLayer::randomItemArray()
{
	if (!m_isUpdateItemList)
		return;

	//随机1-10之间的十个数的顺序
	for (int i = 0;i< m_pItemListNum;i++)
	{
		m_itemRandomIndex[i] = i+1;
	}
	random_shuffle(m_itemRandomIndex,m_itemRandomIndex+m_pItemListNum);
	m_isUpdateItemList  = false;

	//for (int i = 0 ;i< m_pItemListNum ;i++)
	//{
	//	int random = rand()%10 + 1;
	//	if (i == 0)
	//	{
	//		m_itemRandomIndex[i] = random;
	//	}else
	//	{
	//		for (int index =0; index< i ;index++)
	//		{
	//			if ()
	//			{
	//			}
	//		}
	//		m_itemRandomIndex[i] = random;
	//	}
	//}
}
void CLuckyWheelLayer::updateLotteryItem()
{
	//更新转盘Item的显示
	CCLOG("updateLotteryItem,_ onceNeedGold = %d",m_onceNeedGold);

	CLabel* onceNeedGold = (CLabel*)m_ui->findWidgetById("onceNeedGold");
	CLabel* onceNeedGold_0 = (CLabel*)m_ui->findWidgetById("onceNeedGold_0");
	if (m_onceNeedGold != 0)
	{
		onceNeedGold->setVisible(true);
		onceNeedGold->setString(ToString(m_onceNeedGold));
		onceNeedGold_0->setVisible(false);
		CImageView* redPoint = (CImageView* )m_ui->findWidgetById("redPoint");
		redPoint->setVisible(false);
	}else
	{
		onceNeedGold->setVisible(false);
		onceNeedGold_0->setVisible(true);
		CImageView* redPoint = (CImageView* )m_ui->findWidgetById("redPoint");
		redPoint->setVisible(true);
	}

	CLabel* tenNeedGold = (CLabel* )m_ui->findWidgetById("tenNeedGold");
	tenNeedGold->setString(ToString(m_tenNeedGold));

	//随机一个数组的值
	randomItemArray();
	for (int i = 0; i < 10; i++)
	{
		CCLOG("prize_index_%d_in_prizeList_index_is_%d",i+1,m_itemRandomIndex[i]);
	}
	for (int i = 0 ; i < m_prizeList.size();i++)
	{
		CPrize& iPrize = m_prizeList.at(m_itemRandomIndex[i] -1);

		CCString* str = CCString::createWithFormat("Item%d",i+1);
		CLayout* item = (CLayout* )m_ui->findWidgetById(str->getCString());
		//道具背景
		CImageView* pImage = (CImageView* )item->getChildByTag(1);
		pImage->removeAllChildren();
		pImage->setTouchEnabled(true);
		pImage->setEnabled(true);
		pImage->setUserData(&m_prizeList.at(i));
		pImage->setOnPressListener(this, ccw_press_selector(CLuckyWheelLayer::onPress));

		CCSprite* spItem = nullptr;
		if(iPrize.thumb >= 0)
		{
			//道具框
			CImageView* frame = (CImageView* )item->getChildByTag(2);
			
			frame->removeAllChildren();
			SmartSetRectPrizeColor(frame, &iPrize);
			spItem = CCSprite::create(GetImageName(iPrize.type,iPrize.color,iPrize.thumb).c_str());
			if (!spItem)
			{
				spItem = CCSprite::create("headImg/101.png");
				CCLOG("CLuckyWheelLayer::updateLotteryItem error load image %d",iPrize.thumb);
			}
			spItem->setPosition(ccp(frame->getContentSize().width/2, frame->getContentSize().height/2));
			frame->addChild(spItem, -1,1);
			spItem->setScale(0.9f);
			//添加数量
			CCLabelAtlas* itemNum = CCLabelAtlas::create("","label/no_02.png",9,15,46);
			itemNum->setAnchorPoint(ccp(1.0f,0.0f));
			itemNum->setPosition(ccp(85,5));
			pImage->addChild(itemNum, 99,99);
			itemNum->setString(ToString(iPrize.num));
			//CLabel* itemNum = (CLabel* )item->getChildByTag(3);
			//itemNum->setString(ToString(iPrize.num));
			if (iPrize.num <= 1)
			{
				itemNum->setVisible(false);
			}
			
			//CLabel* itemName = (CLabel* )item->getChildByTag(4);
			//itemName->setString(ToString(iPrize.id));

			if (iPrize.quality > 0)
			{
				CLayout* pStar = SmartGetStarLayout(&iPrize);
				frame->addChild(pStar,20,3);
			}
		}

	}
}
void CLuckyWheelLayer::updateUserData()
{
	//更新玩家资源信息
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	for (int i = 0; i < m_getPrizeList.size(); i++)
	{
		const CPrize &prize = m_getPrizeList.at(i);
		if (prize.type==3)
		{
			user->setCoin(user->getCoin()+prize.num);
		}
		else if (prize.type==4)
		{
			user->setRoleGold(user->getRoleGold()+prize.num);
		}
		else if (prize.type==5)
		{
			user->setRoleFood(user->getRoleFood()+prize.num);
		}
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);
}

//CCObject* CLuckyWheelLayer::tableviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
//{
//	CTableViewCell* pCell = (CTableViewCell*)pConvertCell;
//	if (!pCell)
//	{
//		pCell = new CTableViewCell;
//		pCell->autorelease();
//	}else
//	{
//		pCell->removeAllChildren();
//	}
//	addtableViewCell(pCell,uIdx);
//
//	return pCell;
//}
//void CLuckyWheelLayer::addtableViewCell(CTableViewCell* pCell, unsigned int uIdx)
//{
//	//if (m_noticeList.size() == 0) return;
//	CNotice* data = &m_noticeList.at(uIdx);
//
//	CLayout* lay = UICloneMgr::cloneLayout(m_cell);
//
//	for (int i = 1; i<= 6 ;++i)
//	{
//		CCNode* child = lay->getChildByTag(i);
//		lay->removeChild(child);
//		pCell->addChild(child);
//		child->setVisible(false);
//
//		CLabel* notice = (CLabel*)child;
//		switch (i)
//		{
//		case 4:
//			notice->setString(data->player.c_str());
//			break;
//		case 5:
//			{
//
//				const ItemData * itemData = nullptr;
//				itemData = DataCenter::sharedData()->getItemDesc()->getCfg(data->id);
//				if (itemData)
//				{
//					if (itemData->itemName.c_str())
//						notice->setString(itemData->itemName.c_str());
//				}
//
//				notice->setString(itemData->itemName.c_str());
//				//notice->setString(ToString(data->id));
//			}
//
//			break;
//		case 6:
//			notice->setString(ToString(data->num));
//			break;
//		default:
//			break;
//		}
//	}
//}

void CLuckyWheelLayer::initNoticeLabelShow()
{
	//初始化公告列表信息

	CImageView* info = (CImageView* )m_ui->findWidgetById("info");

	m_minPos = info->getPositionY() - info->getContentSize().height/2;
	m_maxPos = info->getPositionY() + info->getContentSize().height/5;
	m_showNoticeLabNum = 20;

	CLayout* layout = (CLayout* )m_ui->findWidgetById("NoticeLabel");
	for (int i =1; i<= m_showNoticeLabNum; i++)
	{
		CLayout* cloneLay = UICloneMgr::cloneLayout(layout);
		m_ui->addChild(cloneLay,-9);
		cloneLay->setAnchorPoint(ccp(0,0));
		cloneLay->setPositionX(CCDirector::sharedDirector()->getWinSize().width);
		cloneLay->setVisible(false);
		m_noticeLab.push_back(cloneLay);
	}
}
void CLuckyWheelLayer::updateNoticeLabelShow()
{
	//更新公告显示奖励Label

	if (m_noticeList.size() < m_showNoticeLabNum)
	{
		//m_noticeList.size()
		for (int i = 0; i< m_noticeList.size(); i++)
		{
			CLayout* lay = m_noticeLab.at(i);
			CLabel* name = (CLabel* )lay->findWidgetById("Name");
			CLabel* prize = (CLabel* )lay->findWidgetById("Prize");
			CLabel* num = (CLabel* )lay->findWidgetById("Num");

			CNotice* data = &m_noticeList.at(i);
			const ItemData * itemdate = nullptr;
			itemdate = DataCenter::sharedData()->getItemDesc()->getCfg(data->id);
			CCString* strName = CCString::createWithFormat("%s",data->player.c_str());
			CCString* strPrize = CCString::createWithFormat("%s",itemdate->itemName.c_str());
			CCString* strNum = CCString::createWithFormat("%d",data->num);
			name->setString(strName->getCString());
			prize->setString(strPrize->getCString());
			num->setString(strNum->getCString());
		}
		for (int i = m_noticeList.size(); i < m_showNoticeLabNum ; i++)
		{
			//CLabel* lab = m_noticeLab.at(i);
			//int randomNum = rand()%(m_noticeList.size());

			//CNotice* data = &m_noticeList.at(randomNum);
			//const ItemData * itemData = nullptr;
			//itemData = DataCenter::sharedData()->getItemDesc()->getCfg(data->id);

			//CCString* str = CCString::createWithFormat("恭喜_%s_获得_%s*%d",data->player.c_str(),itemData->itemName.c_str(),data->num);
			//lab->setString(str->getCString());
		}
	}
	else
	{

	}
}
void CLuckyWheelLayer::updateNoticeLabelShowSchedule(float delta)
{
	//调度器移动公告信息
	CLabel* pLabel = (CLabel*)m_ui->findWidgetById("Label");

	++m_showNoticeLabNum;
	if (m_showNoticeLabNum > 20)
		m_showNoticeLabNum -= 20;
	for (int i = 0; i <m_noticeLab.size(); i++)
	{
		if (m_showNoticeLabNum == i + 1)
		{
			CLayout* lab = m_noticeLab.at(i);
			lab->setPositionX(VRIGHT - 300);
			lab->setPositionY(m_minPos);
			//CCLOG("%f,%f",lab->getContentSize().width,lab->getContentSize().height);
			//CCLOG("%f,%f",lab->getPositionX(),lab->getPositionY());
			lab->setVisible(true);
			lab->runAction(CCSequence::create(
				CCMoveTo::create(10.0f,ccp(VRIGHT - 300,m_maxPos)),
				CCHide::create(),
				nullptr));
		}
	}
}

void CLuckyWheelLayer::onExit()
{
	BaseLayer::onExit();
	
	GetTcpNet->unRegisterAllMsgHandler(this);
	this->unschedule(schedule_selector(CLuckyWheelLayer::updateNoticeLabelShowSchedule));
	this->unschedule(schedule_selector(CLuckyWheelLayer::updateLightActionEffect));
	m_noticeList.clear();
	m_prizeList.clear();
}