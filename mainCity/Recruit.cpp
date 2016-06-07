#include "model/DataCenter.h"
#include "Global.h"
#include "common/color.h"
#include "GMessage.h"
#include "netControl/CPlayerControl.h"
#include "tools/ShowTexttip.h"
#include "scene/layer/BackLayer.h"
#include "mainCity/mainCityUI.h"
#include "mainCity/MainCityControl.h"
#include "Recruit.h"
#include "scene/CPopTip.h"
#include "tools/UICloneMgr.h"
#include "net/CNetClient.h"
#include "scene/AnimationManager.h"
#include "scene/alive/AliveDefine.h"
//#include "scene/layer/LayerDefine.h"
#include "RecruitResult.h"
#include "common/CheckMoney.h"
#include "common/MultLanguage.h"
#include "tollgate/MonsterInfo.h"
#include "common/CGameSound.h"
#include "common/CShowToBuyResource.h"
#include "common/MultLanguage.h"
#include "tollgate/ItemInfo.h"
#include "common/CommonFunction.h"

#define TileMapWidth  1920
#define TileMapHeight 640

CRecruit::CRecruit()
	:m_Recruit(false),m_HeroType(0),m_ui(nullptr),m_RecruiLayer(nullptr)
	,m_SendType(0), m_pMonsterInfo(nullptr),m_pItemInfo(nullptr)
{}
using namespace BattleSpace;
CRecruit::~CRecruit()
{
	m_LotteryData.clear();
}
bool CRecruit::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("Rercuit");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("recruit.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		return true;
	}
	return false;
}
void CRecruit::ReturnCity(CCObject* ob)
{
	PlayEffectSound(SFX_Ensure);
	LayerManager::instance()->pop();
	LayerManager::instance()->pop();
}
void CRecruit::onExit()
{
	BaseLayer::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	GetTcpNet->unRegisterAllMsgHandler(this);
	NOTIFICATION->removeAllObservers(this);
	NOTIFICATION->postNotification(SHOW_MAIN_SCENE);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}
void CRecruit::onEnter()
{
	GetTcpNet->sendDataType(HeroLottery);
	BaseLayer::onEnter();

	//退出
	CButton* pClose = CButton::create("common/back.png", "common/back.png");
	pClose->getSelectedImage()->setScale(1.1f);
	pClose->setPosition(m_ui->convertToNodeSpace(ccp(VLEFT+50, VTOP-50)));
	pClose->setId("close");
	pClose->setOnClickListener(this,ccw_click_selector(CRecruit::ReturnCity));
	m_ui->addChild(pClose, 999);


	m_RecruiLayer = (CLayout*)(m_ui->findWidgetById("garrsion"));
	m_heroLayer = (CLayout*)(m_ui->findWidgetById("hero"));

	m_ui->setScale(0.9f);
	m_ui->setOpacity(0);
	CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	CCFadeTo *alpha = CCFadeTo::create(0.3f,255);
	m_ui->runAction(big);
	m_ui->runAction(alpha);
	m_heroLayer->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f),CCScaleTo::create(0.2f,1.05f)));

	GetTcpNet->registerMsgHandler(HeroLottery,this, CMsgHandler_selector(CRecruit::initRecruiData));
	GetTcpNet->registerMsgHandler(LotteryHeroData,this, CMsgHandler_selector(CRecruit::HeroLotteryInfo));

	CCSprite *door1 = (CCSprite*)(m_ui->findWidgetById("door1"));
	CCSprite *door2 = (CCSprite*)(m_ui->findWidgetById("door2"));
	CCSprite *door3 = (CCSprite*)(m_ui->findWidgetById("door3"));

	{
		CCAnimation *fireAnim = AnimationManager::sharedAction()->getAnimation("8006");
		fireAnim->setDelayPerUnit(0.1f);
		CCPoint pos = ccp(164.5f, 186);
		CImageView *fire = CImageView::create("skill/8006.png");
		fire->setTouchEnabled(true);
		fire->runAction(CCRepeatForever::create(CCAnimate::create(fireAnim)));
		fire->setPosition(pos);
		m_RecruiLayer->addChild(fire,0,204);
	}
	{
		CCAnimation *fireAnim = AnimationManager::sharedAction()->getAnimation("8007");
		fireAnim->setDelayPerUnit(0.1f);
		CCPoint pos = ccp(557, 208);
		CImageView *fire = CImageView::create("skill/8007.png");
		fire->setTouchEnabled(true);
		fire->runAction(CCRepeatForever::create(CCAnimate::create(fireAnim)));
		fire->setPosition(pos);
		m_RecruiLayer->addChild(fire,0,206);
	}
	{
		CCAnimation *fireAnim = AnimationManager::sharedAction()->getAnimation("8008");
		fireAnim->setDelayPerUnit(0.1f);
		CCPoint pos = ccp(949, 194);
		CImageView *fire = CImageView::create("skill/8008.png");
		fire->setTouchEnabled(true);
		fire->runAction(CCRepeatForever::create(CCAnimate::create(fireAnim)));
		fire->setPosition(pos);
		m_RecruiLayer->addChild(fire,0,205);
	}

	//绑定场景隐藏和显示的消息
	NOTIFICATION->addObserver(this, callfuncO_selector(CRecruit::show), SHOW_RECRUIT, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CRecruit::hide), HIDE_RECRUIT, nullptr);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(CRecruit::roleUpdate));
}

//初始化抽奖界面 //6
void CRecruit::InitRecrui(int num1,int num2,int num3)
{
	CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
	CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
	CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
	Num1->setString(ToString(num1));
	Num2->setString(ToString(num2));
	Num3->setString(ToString(num3));
	if(num2==0)
	{
		Num2->setString(GETLANGSTR(1116));
	}

	roleUpdate(*(TMessage*)(nullptr));

	for (int i=FriendshipBtnRec;i<= TenBtnRec;i++)
	{
		CImageView* button = (CImageView*)m_RecruiLayer->getChildByTag(i);
		button->setTouchEnabled(true);
		button->setOnClickListener(this,ccw_click_selector(CRecruit::LotteryBtn));
	}
	for (int i=0;i<m_LotteryData.size();i++)
	{
		char str1 [60] = {0};
		char str2 [60] = {0};

		const HeroLotteryData& lotteryData = m_LotteryData.at(i);

		int type = lotteryData.type;

		//获取品质图片
		CImageView* pQuality = (CImageView*)m_heroLayer->getChildByTag(One_HeroType+10*i);

		//CLabel* name = (CLabel*)m_heroLayer->getChildByTag(One_HeroName+10*i);

		CImageView* pHeadBg = (CImageView*)m_heroLayer->getChildByTag(One_ImgRec+10*i);


		//类型默认不显示
		CImageView* roleType = (CImageView*)m_heroLayer->getChildByTag(One_NewImgRec+10*i);
		roleType->setVisible(false);

		//英雄类型 或 魂石
		//type1， 英雄， 
		//type2,    iColor >=10	:魂石,秘籍
		//type2,    iColor <10	装备

		//设置品质-颜色
		int iAdapterType = lotteryData.type;
		iAdapterType = iAdapterType==1?2:1;
		pQuality->setTexture(setItemQualityTexture(lotteryData.iColor));
		pQuality->setUserData(&m_LotteryData.at(i));
		pQuality->setTouchEnabled(true);
		pQuality->setOnPressListener(this, ccw_press_selector(CRecruit::onPress));

		//英雄
		if(type == 1)
		{
			//图片从headImage里面拿
			sprintf(str1,"headImg/%d.png", lotteryData.thumb);
			//读取数据
			const HeroInfoData *data = DataCenter::sharedData()->getHeroInfo()->getCfg(lotteryData.thumb);
			if(data)
			{
				//if(data->heroName.size()>0)
				//{
				//	//设置名称
				//	name->setString(data->heroName.c_str());
				//}
				//职业图片
				roleType->setVisible(true);
				roleType->setTexture(CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("common/type_%d_%d.png", lotteryData.heroType, data->iType2)->getCString()));
			}
		}
		//魂石
		else if(type == 2 && lotteryData.iColor >= 10)
		{
			//图片从headImage里面拿
			sprintf(str1,"headImg/%d.png", lotteryData.thumb);
			////读取数据
			//const ItemData *data = DataCenter::sharedData()->getItemDesc()->getCfg(lotteryData.id);
			//if(data)
			//{
			//	if(data->itemName.size()>0)
			//	{
			//		//设置名称
			//		name->setString(data->itemName.c_str());
			//	}
			//}
		}
		//装备
		else if(type == 2 && lotteryData.iColor < 10)
		{
			//图片从prop里面拿
			sprintf(str1, "prop/%d.png", lotteryData.thumb);
			////读取数据
			//const ItemData *data = DataCenter::sharedData()->getItemDesc()->getCfg(lotteryData.id);
			//if(data)
			//{
			//	if(data->itemName.size()>0)
			//	{
			//		//设置名称
			//		name->setString(data->itemName.c_str());
			//	}
			//}
		}

		//添加星级
		if(lotteryData.quality>0)
		{
			CLayout* pStarLayout = getStarLayout(lotteryData.quality);
			pQuality->addChild(pStarLayout);
		}

		//添加触摸事件
		CImageView* pHeroImage = CImageView::create(str1);
		if(!pHeroImage)
		{
			pHeroImage = CImageView::create("headImg/101.png");
			CCLOG("void CRecruit::InitRecrui-%s", str1);
		}
		pHeadBg->addChild(pHeroImage);
		NodeFillParent(pHeroImage);

	}

}

void CRecruit::showRedTip(bool isVisable)
{
	CCSprite *redPoint = (CCSprite*)m_ui->findWidgetById("redPoint");
	redPoint->setVisible(isVisable);
}

//抽奖按钮回调
void CRecruit::LotteryBtn(CCObject* ob)
{
	//点击之前判断元宝或友情点是否足够
	CButton* btn = (CButton*)ob;
	int type = btn->getTag();
	UserData* data = DataCenter::sharedData()->getUser()->getUserData();
	CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
	CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
	CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
	m_SendType = type;
	switch (m_SendType)
	{
	case FriendshipBtnRec:	
		{ 
			if(CheckFriendShip(atoi(Num1->getString())))
			{
				CPlayerControl::getInstance().SendLotteryType(FriendshipOnes);
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(1016));
			}
		} break;
	case OnecBtnRec:		
		{ 
			if(CheckGold(atoi(Num2->getString())))
			{	
				int gold = atoi(Num2->getString());
				if (gold>0)
				{
					CCString *str = CCString::createWithFormat(GETLANGSTR(1027),gold);				
					ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(CRecruit::onComfirmOneTime));
				}
				else
				{
					CPlayerControl::getInstance().SendLotteryType(GoldOnes);
				}	
				showRedTip(false);
			}
			else
			{
				//ShowPopTextTip(GETLANGSTR(203));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
			}
		} break;
	case TenBtnRec:			
		{ 
			if(CheckGold(atoi(Num3->getString())))
			{
				CCString *str = CCString::createWithFormat(GETLANGSTR(1028), atoi(Num3->getString()));
				ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(CRecruit::onComfirmTenTime));
				//CPlayerControl::getInstance().SendLotteryType(GoldTen);
			}
			else
			{
				//ShowPopTextTip(GETLANGSTR(203));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
			}
		} break;
	default: 
		break;
	}
}

void CRecruit::onComfirmOneTime(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		CPlayerControl::getInstance().SendLotteryType(GoldOnes);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void CRecruit::onComfirmTenTime(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==PopTipConfirm)
	{
		CPlayerControl::getInstance().SendLotteryType(GoldTen);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

//接收抽奖返回信息
void CRecruit::HeroLotteryInfo(int type, google::protobuf::Message *msg)
{
	if (type != LotteryHeroData)
		return;
	LotteryResponse* res = (LotteryResponse*)msg;
	this->setVisible(true);

	switch (res->result())
	{
	case LotterySucceed:
		{
			//ShowTexttip("[ Tips : LotterySucceed ]",ccc3(255,0,0),0,ccp(480,320),0,1.5f,0,200,FONT_NAME,35);
			upRoleData();				//扣减元宝(友情点),刷新武将数据
			vector<HeroLotteryData> Results;
			int i=0;
			for ( ;i<res->prizelist_size(); i++)
			{
				Lottery lottery = res->prizelist(i);
				HeroLotteryData data;
				data.readData(lottery);
				Results.push_back(data);
			}
			if (Results.size())
			{
				CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
				CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
				CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
				RecruitResult *recr = RecruitResult::create();
				int num = 0;
				switch (m_SendType)
				{
				case FriendshipBtnRec:	{ num =  atoi(Num1->getString()); }break;
				case OnecBtnRec:		{ num =  atoi(Num2->getString()); }break;
				case TenBtnRec:			{ num =  atoi(Num3->getString()); }break;
				default:
					break;
				}
				recr->setResult(Results,m_SendType,res->price());
				//ReturnCity(nullptr);
				LayerManager::instance()->push(recr);
			}
		}
		break;
	case NeedFriendship:
		ShowTexttip("[ Tips : NeedFriendship ]",ccc3(255,0,0),0,ccp(480,320),0,1.5f,0,200,FONT_NAME,35);
		break;
	case NeedGold:
		ShowTexttip("[ Tips : NeedGold ]",ccc3(255,0,0),0,ccp(480,320),0,1.5f,0,200,FONT_NAME,35);
		break;
	case DataError:
		ShowTexttip("[ Tips : DataError ]",ccc3(255,0,0),0,ccp(480,320),0,1.5f,0,200,FONT_NAME,35);
		break;
	default:
		break;
	}
}
//抽奖成功刷新武将信息
void CRecruit::upRoleData()
{
	UserData* data = DataCenter::sharedData()->getUser()->getUserData();
	CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
	CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
	CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
	switch (m_SendType)
	{
	case FriendshipBtnRec:	{ data->setFriends(data->getFriends()-atoi(Num1->getString())); }break;
	case OnecBtnRec:		{ data->setRoleGold(data->getRoleGold()-atoi(Num2->getString())); }break;	
	case TenBtnRec:			{ data->setRoleGold(data->getRoleGold()-atoi(Num3->getString())); }break;
	default:
		break;
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);//刷新武将
}

void CRecruit::updateCostNum(int num)
{
	CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
	CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
	CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
	switch (m_SendType)
	{
	case FriendshipBtnRec:	{ Num1->setString(ToString(num)); }break;
	case OnecBtnRec:		{ Num2->setString(ToString(num)); }break;	
	case TenBtnRec:			{ Num3->setString(ToString(num)); showFreeTime(); }break;
	default:
		break;
	}
	roleUpdate(*(TMessage*)(nullptr));
}

//接收初始化抽奖信息
void CRecruit::initRecruiData(int type, google::protobuf::Message *msg)
{
	if (type != HeroLottery)
		return;
	
	if(!isVisible())
	{
		this->setVisible(true);
		NOTIFICATION->postNotification(HIDE_MAIN_SCENE);
		PlayEffectSound(SFX_416);
	}

	LotteryData *res = (LotteryData*)msg;

	int i=0;
	for ( ;i<res->prizelist_size(); i++)
	{
		Lottery lottery = res->prizelist(i);
		HeroLotteryData data;
		data.readData(lottery);
		m_LotteryData.push_back(data);
	}
	if (m_LotteryData.size())
		InitRecrui(res->price_1(),res->price_2(),res->price_3());

	if (res->price_3()>0)
	{
		showFreeTime();
	}
	else
	{
		CLabel *freeTime = (CLabel*)(m_ui->findWidgetById("free"));
        freeTime->setVisible(false);
	}
}

void CRecruit::onPress( CCObject* pSender, CTouchPressState iState )
{
	CImageView* pImage = (CImageView*)pSender;
	HeroLotteryData* LotteryData = (HeroLotteryData*)pImage->getUserData();

	if(LotteryData->type == 1)
	{
		if(m_pMonsterInfo==nullptr)
		{
			m_pMonsterInfo = CMonsterInfo::create();
			this->addChild(m_pMonsterInfo);
		}

		switch (iState)
		{
		case CTouchPressOn:
			{
				PlayEffectSound(SFX_429);

				CHero pHero;
				pHero.heroid = LotteryData->id;
				pHero.thumb = LotteryData->thumb;
				pHero.quality = LotteryData->quality;
				pHero.iColor = LotteryData->iColor;
				pHero.level = 1;
				m_pMonsterInfo->setHero(&pHero);
				m_pMonsterInfo->bindPos(pImage);
				m_pMonsterInfo->show();
				CCLOG("show");
			}
			break;
		case CTouchPressOff:
			{
				m_pMonsterInfo->hide();
				CCLOG("hide");
			}
			break;
		default:
			break;
		}
	}
	else
	{

		if(m_pItemInfo == nullptr)
		{
			m_pItemInfo = CItemInfo::create();
			this->addChild(m_pItemInfo);
		}

		switch (iState)
		{
		case CTouchPressOn:
			{
				PlayEffectSound(SFX_429);

				CPrize prize;
				prize.id = LotteryData->id;
				prize.thumb = LotteryData->thumb;
				prize.quality = LotteryData->quality;
				prize.color = LotteryData->iColor;
				m_pItemInfo->setInfo(&prize);
				m_pItemInfo->bindPos(pImage);
				m_pItemInfo->show();
				CCLOG("show");
			}
			break;
		case CTouchPressOff:
			{
				m_pItemInfo->hide();
				CCLOG("hide");
			}
			break;
		default:
			break;
		}
	}


}

void CRecruit::show( CCObject* pObj )
{
	setVisible(true);
	GetTcpNet->registerMsgHandler(LotteryHeroData,this, CMsgHandler_selector(CRecruit::HeroLotteryInfo));
}

void CRecruit::hide( CCObject* pObj )
{
	setVisible(false);
}

void CRecruit::roleUpdate( const TMessage& tMsg )
{
	CLabel* Num1 = (CLabel*)m_RecruiLayer->getChildByTag(FriendshipNumRec);
	CLabel* Num2 = (CLabel*)m_RecruiLayer->getChildByTag(OnceGoldNumRec);
	CLabel* Num3 = (CLabel*)m_RecruiLayer->getChildByTag(TenGoldNumRec);
	//检查货币数量
	if(!CheckFriendShip(atoi(Num1->getString())))
	{
		Num1->setColor(RGB_RED);
	}
	else
	{
		Num1->setColor(RGB_WHITE);
	}

	//检查货币数量
	if(!CheckGold(atoi(Num2->getString())))
	{
		Num2->setColor(RGB_RED);
	}
	else
	{
		Num2->setColor(RGB_WHITE);
	}

	//检查货币数量
	if(!CheckGold(atoi(Num3->getString())))
	{
		Num3->setColor(RGB_RED);
	}
	else
	{
		Num3->setColor(RGB_WHITE);
	}
}

void CRecruit::showFreeTime()
{
	time_t tnow = time(nullptr);
	tm tim;
	tim =*localtime(&tnow);
	int hour = 23 - tim.tm_hour;
	int min = 60- tim.tm_min;
	CLabel *freeTime = (CLabel*)(m_ui->findWidgetById("free"));
	char str[60]={0};
	sprintf(str,"%d Min ",hour*60+min);
	string pstr(str);
	string qstr(GETLANGSTR(1120));
	pstr = pstr + qstr;
	freeTime->setString(pstr.c_str());
	freeTime->setVisible(true);
	this->schedule(schedule_selector(CRecruit::updateTime),60);
}

void CRecruit::updateTime(float dt)
{
	time_t tnow = time(nullptr);
	tm tim;
	tim =*localtime(&tnow);
	int hour = 23 - tim.tm_hour;
	int min = 60- tim.tm_min;
	CLabel *freeTime = (CLabel*)(m_ui->findWidgetById("free"));
	char str[60]={0};
	sprintf(str,"%d Min ",hour*60+min);
	string pstr(str);
	string qstr(GETLANGSTR(1120));
	pstr = pstr + qstr;
	freeTime->setString(pstr.c_str());
}