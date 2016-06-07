#include "RecruitResult.h"
#include "scene/effect/EffectObject.h"
#include "GMessage.h"
#include "common/CommonFunction.h"
#include "bag/bagData.h"
#include "tools/UICloneMgr.h"
#include "model/DataCenter.h"
#include "tools/ShowTexttip.h"
#include "netControl/CPlayerControl.h"
#include "GMessage.h"
#include "net/CNetClient.h"
#include "scene/AnimationManager.h"
#include "guide/GuideManager.h"
#include "Resources.h"
#include "common/CGameSound.h"
#include "common/CShowToBuyResource.h"
#include "scene/CPopTip.h"
#include "mainCity/CNewHero.h"
#include "tollgate/MonsterInfo.h"
#include "tollgate/ItemInfo.h"
#include "Recruit.h"
#include "common/CheckMoney.h"

RecruitResult::RecruitResult()
	:m_ui(nullptr),m_cell1(nullptr),m_cell2(nullptr),m_Res(nullptr),m_Turn(nullptr)
	,m_Type(0),m_needNum(0),m_EFView(nullptr),m_HeroView(nullptr), m_BtnTurn(false)
	,m_cellIndex(0)
	,m_iLotteryRank(0)
	,m_armature(nullptr)
	,m_pNewHeroEffect(nullptr)
	,m_pMonsterInfo(nullptr)
	,m_pItemInfo(nullptr)
	,m_bIsSuccess(false)
{}
using namespace BattleSpace;
RecruitResult::~RecruitResult()
{
	m_LotteryResult.clear();
	CC_SAFE_RELEASE(m_cell1);
	CC_SAFE_RELEASE(m_cell2);
}

bool RecruitResult::init()
{
	if (BaseLayer::init())
	{
		MaskLayer* lay = MaskLayer::create("RercuitResult");
		lay->setContentSize(CCSizeMake(2824,640));
		LayerManager::instance()->push(lay);

		m_ui = LoadComponent("LotteryResult.xaml");  //
		m_ui->setPosition(VCENTER);
		this->addChild(m_ui);
		this->setVisible(true);

		m_pNewHeroEffect = CNewHero::create();
		m_pNewHeroEffect->setVisible(false);
		this->addChild(m_pNewHeroEffect, 20);

		return true;
	}
	return false;
}


void RecruitResult::onExit()
{
	BaseLayer::onExit();
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	GetTcpNet->unRegisterAllMsgHandler(this);

	CCArmatureDataManager::sharedArmatureDataManager()->removeAnimationData("recruit/zhaomu.ExportJson");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r1.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r2.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r3.plist");
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();

	NOTIFICATION->postNotification(SHOW_RECRUIT);
	NOTIFICATION->postNotification(SHOW_TOP_LAYER);

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);
}

void RecruitResult::setResult(vector<HeroLotteryData>& result,int type,int num) 
{ 
	m_LotteryResult = result; 
	m_Type = type;
	m_needNum = num;
	
	//for(unsigned int i=0; i<m_LotteryResult.size(); i++)
	//{
	//	if(i==3 || i==6 || i==9 || i==0)
	//		m_LotteryResult.at(i).isnew = true;
	//}

	checkRank();

	showCoffinWithRank();
}
//进入界面的时候初始化它
void RecruitResult::onEnter()
{
	BaseLayer::onEnter();

	NOTIFICATION->postNotification(HIDE_TOP_LAYER);

	m_ui->setScale(0.0f);
	CCScaleTo *big = CCScaleTo::create(0.2f,1.0f);
	m_ui->runAction(big);

	m_cell1 = (CLayout*)m_ui->getChildByTag(cell1);
	m_cell2 = (CLayout*)m_ui->getChildByTag(cell2);
	m_Res	= (CLayout*)m_ui->getChildByTag(resultLayerRes);
	m_Turn	= (CLayout*)m_ui->getChildByTag(heroIconLayerRes);

	m_cell1->retain();
	m_ui->removeChild(m_cell1);
	m_cell2->retain();
	m_ui->removeChild(m_cell2);

	//特效view
	m_EFView = (CGridView*)m_Res->getChildByTag(resViewRes);
	m_EFView->setDirection(eScrollViewDirectionVertical);	
	m_EFView->setCountOfCell(0);
	m_EFView->setSizeOfCell(m_cell1->getContentSize());
	m_EFView->setAnchorPoint(ccp(0,0));
	m_EFView->setColumns(5);
	m_EFView->setAutoRelocate(true);
	m_EFView->setDeaccelerateable(false);
	m_EFView->setDragable(false);
	m_EFView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(RecruitResult::EFviewDataSource));
	m_EFView->reloadData();

	//10连抽结果view
	m_HeroView = (CGridView*)m_Turn->getChildByTag(heroViewRes);
	m_HeroView->setDirection(eScrollViewDirectionVertical);	
	m_HeroView->setCountOfCell(0);
	m_HeroView->setSizeOfCell(m_cell2->getContentSize());
	m_HeroView->setAnchorPoint(ccp(0,0));
	m_HeroView->setColumns(5);
	m_HeroView->setAutoRelocate(true);
	m_HeroView->setDeaccelerateable(false);
	m_HeroView->setDragable(false);
	m_HeroView->setDataSourceAdapter(this,ccw_datasource_adapter_selector(RecruitResult::HeroviewDataSource));
	m_HeroView->reloadData();

	GetTcpNet->registerMsgHandler(LotteryHeroData,this, CMsgHandler_selector(RecruitResult::ReceiveInfo));

	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("recruit/zhaomu.ExportJson");

	resetArmature();

	CImageView* pView = (CImageView*)m_ui->findWidgetById("hide_btn");
	pView->setTouchEnabled(true);
	pView->setOnClickListener(this, ccw_click_selector(RecruitResult::touchCoffin));
	pView->setOnPressListener(this, ccw_press_selector(RecruitResult::pressCoffin));

	NOTIFICATION->postNotification(HIDE_RECRUIT);

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO, this, GameMsghandler_selector(RecruitResult::roleUpdate));
}

void RecruitResult::openMovementEvent( CCArmature *pArmature, MovementEventType iType, const char *name )
{
	switch (iType)
	{
	case cocos2d::extension::START:
		{
			if(strcmp(name, "ready")==0)
			{
				
			}
		}
		break;
	case cocos2d::extension::COMPLETE:
		{
			if(strcmp(name, "ready")==0)
			{
				pArmature->getAnimation()->play("opened", 0.01f);
				showArrowAndButton();
			}
			else if(strcmp(name, "expo")==0)
			{
				pArmature->getAnimation()->play("fire", 0.01f);

				showHero();
			}
		}
		break;
	case cocos2d::extension::LOOP_COMPLETE:
		break;
	default:
		break;
	}
}


void RecruitResult::openFrameEvent( CCBone *pBone, const char* sKey, int iA, int iB )
{
	//开馆成功
	if(strcmp(sKey, "success")==0)
	{
		m_bIsSuccess = true;

		//箭头
		CCNode* pArrow = (CCNode*)m_ui->findWidgetById("arrow");
		pArrow->setVisible(false);
		CImageView* pView = (CImageView*)m_ui->findWidgetById("hide_btn");
		pView->setVisible(false);

		PlayEffectSound(SFX_425);

	}
}


void RecruitResult::tensRecruit()
{
	//购买十个成功
	//购买十个成功
	m_Res->getChildByTag(resTenLabRes)->setVisible(true);
	m_Res->getChildByTag(resOenLabRes)->setVisible(false);
	m_Res->setVisible(true);

	CLayout* layer = (CLayout*)m_ui->getChildByTag(heroIconLayerRes);
	layer->setPositionY(VCENTER.y);

	//设置cell数量
	m_EFView->setCountOfCell(m_LotteryResult.size());
	m_EFView->reloadData();

	this->scheduleOnce(schedule_selector(RecruitResult::netGetRes),1.2f);
}

void RecruitResult::onesRecruit()						//抽奖一次处理
{
	//抽一次标签
	m_Res->getChildByTag(resTenLabRes)->setVisible(false);
	m_Res->getChildByTag(resOenLabRes)->setVisible(true);
	m_Res->setVisible(true);
	
	CLayout* layer = (CLayout*)m_ui->getChildByTag(heroIconLayerRes);
	layer->setPositionY(VCENTER.y);

	//标记位置
	CCPoint point = /*m_Res->getChildByTag(resImgRes)->getPosition()*/ccp(574, 388);
	m_Res->removeChildByTag(resImgRes);

	this->scheduleOnce(schedule_selector(RecruitResult::netGetRes),1.2f);

	int animId = 8033;	
	if (m_LotteryResult.at(0).iColor < 10)
		animId = (m_LotteryResult.at(0).iColor-1)*10 +8003;

	CCPoint orpos = ccp(point.x-20, point.y-130);

	//灵魂
	CCAnimation *soulAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
	CCSprite *soul = createAnimationSprite(CCString::createWithFormat("skill/%d.png",animId)->getCString(),orpos,soulAnim,true);
	soul->setTag(resImgRes);
	soul->setScale(0.3f);
	m_Res->addChild(soul);
	CCMoveTo *move = CCMoveTo::create(0.3f, ccp(point.x-10, point.y+20));
	soul->runAction(CCSpawn::createWithTwoActions(CCEaseBackOut::create(move), CCScaleTo::create(0.3f, 0.8f)));
	PlayEffectSound(SFX_422);
}

void RecruitResult::ResBtnCallBack(CCObject* ob)
{
	CCNode* node = (CCNode*)ob;
	int btnTag = node->getTag();
	switch (btnTag)
	{
	case newBuyBtnRes:{ BuyAgain(); }break;
	case newSureBtnRes:
		{
			CRecruit *recruit = (CRecruit*)(LayerManager::instance()->getLayer("CRecruit"));
			recruit->updateCostNum(m_needNum);
			LayerManager::instance()->pop();			//让界面消失的方法
			LayerManager::instance()->pop();
		}break;
	default:
		break;
	}

}

void RecruitResult::BuyAgain()
{
	UserData* data = DataCenter::sharedData()->getUser()->getUserData();

	switch (m_Type)
	{
	case FriendshipBtnRec:	
		{ 
			if (data->getFriends() > m_needNum)
			{
				CPlayerControl::getInstance().SendLotteryType(FriendshipOnes);
			}
			else
			{
				ShowPopTextTip(GETLANGSTR(1016));
				return;
			}
		} 
		break;
	case OnecBtnRec:		
		{ 
			if (data->getRoleGold() > m_needNum)
			{
				//CPlayerControl::getInstance().SendLotteryType(GoldOnes);
				CCString *str = CCString::createWithFormat(GETLANGSTR(1027),m_needNum);				
				ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(RecruitResult::onComfirmOneTime));
			}
			else
			{
				//ShowPopTextTip(GETLANGSTR(203));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
				return;
			}
		} 
		break;
	case TenBtnRec:			
		{ 
			if (data->getRoleGold() > m_needNum)
			{
				//CPlayerControl::getInstance().SendLotteryType(GoldTen);
				CCString *str = CCString::createWithFormat(GETLANGSTR(1028),m_needNum);
				ShowConfirmTextTip(str->getCString(),this,ccw_click_selector(RecruitResult::onComfirmTenTime));
			}
			else
			{
				//ShowPopTextTip(GETLANGSTR(203));
				CShowToBuyResource* pShow = CShowToBuyResource::create();
				pShow->showToBuyResourceByType(ShowBuyResourceGold);
				return;
			}
		} 
		break;
	default: 
		break;
	}
}

void RecruitResult::onComfirmOneTime(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		CPlayerControl::getInstance().SendLotteryType(GoldOnes);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void RecruitResult::onComfirmTenTime(CCObject* pSender)
{
	CButton *btn = (CButton*)pSender;
	if (btn->getTag()==0)
	{
		CPlayerControl::getInstance().SendLotteryType(GoldTen);
	}
	((CPopTip*)(btn->getParent()->getParent()))->onClose(nullptr);
}

void RecruitResult::ReceiveInfo(int type, google::protobuf::Message *msg)
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
			AgainUpRoleData();				//扣减元宝(友情点),刷新武将数据
			vector<HeroLotteryData> Results;
			int i=0;
			for ( ;i<res->prizelist_size(); i++)
			{
				Lottery lottery = res->prizelist(i);
				HeroLotteryData data;
				data.readData(lottery);
				Results.push_back(data);
			}

			m_LotteryResult.clear();
			m_LotteryResult = Results;

			//隐藏之前的
			m_EFView->getContainer()->removeAllChildren();
			m_HeroView->getContainer()->removeAllChildren();
			m_Turn->setVisible(false);

			//CLayout* comLayer = (CLayout*)m_Turn->getChildByTag(commonLayerRes);
			//CLayout* getLayer = (CLayout*)m_Turn->getChildByTag(heruTurnLayerRes);
			//CLayout* stars	  = (CLayout*)m_Turn->getChildByTag(newStarsLayerRes);
			//CLayout* newLayer = (CLayout*)m_Turn->getChildByTag(newHeroLayerRes);
			//newLayer->setVisible(false);
			//for(unsigned int i=0; i<m_LotteryResult.size(); i++)
			//{
			//	m_LotteryResult.at(i).isnew = true;
			//}
			//for(unsigned int i=0; i<m_LotteryResult.size(); i++)
			//{
			//	if(i==3 || i==6 || i==9 || i==0)
			//		m_LotteryResult.at(i).isnew = true;
			//}

			//找不到在哪，先这样吧
			CLayout* layer = (CLayout*)m_ui->getChildByTag(heroIconLayerRes);
			layer->setPositionY(-9999);

			checkRank();
			showCoffinWithRank();
			resetArmature();
		}
		break;
	case NeedFriendship:
		ShowPopTextTip(GETLANGSTR(1016));
		break;
	case NeedGold:
		{
			//ShowPopTextTip(GETLANGSTR(203));
			CShowToBuyResource* pShow = CShowToBuyResource::create();
			pShow->showToBuyResourceByType(ShowBuyResourceGold);
		}
		break;
	case DataError:
		ShowPopTextTip(GETLANGSTR(207));
		break;
	default:
		break;
	}
}

void RecruitResult::AgainUpRoleData()
{
	UserData* data = DataCenter::sharedData()->getUser()->getUserData();
	switch (m_Type)
	{
	case FriendshipBtnRec:	{ data->setFriends(data->getFriends()- m_needNum); }break;
	case OnecBtnRec:		{ data->setRoleGold(data->getRoleGold()- m_needNum); }break;	
	case TenBtnRec:			{ data->setRoleGold(data->getRoleGold()-m_needNum); }break;
	default:
		break;
	}
	CSceneManager::sharedSceneManager()->PostMessageA(UPDATE_HERO,0,nullptr,nullptr);//刷新武将
}

bool RecruitResult::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CWidgetWindow::ccTouchBegan(pTouch,pEvent);
	return true;
}

 void RecruitResult::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
 {
	 CWidgetWindow::ccTouchEnded(pTouch,pEvent);
 }


void RecruitResult::setHeroIcon(CCSprite* icon,CCSprite*quality,CLabel* name,HeroLotteryData& data,CCSprite* newicon)
{
	if (outPutERRORMsg("RecruitResult::setHeroIcon,icon",icon)||
		outPutERRORMsg("RecruitResult::setHeroIcon,quality",quality)||
		outPutERRORMsg("RecruitResult::setHeroIcon,name",name))return;
	char pathIcon[60] = {0};
	icon->setScale(0.9f);

	//设置品质-颜色
	int iAdapterType = data.type;
	iAdapterType = data.type==1?2:1;
	quality->setTexture(setItemQualityTexture(data.iColor));

	if (data.type == 1 || data.iColor >= 10)
	{
		sprintf(pathIcon,"headImg/%d.png",data.thumb);

		if(data.iColor < 10)
		{
			const HeroInfoData *c_data = DataCenter::sharedData()->getHeroInfo()->getCfg(data.thumb);
			if(c_data->heroName.size()>0)
				name->setString(c_data->heroName.c_str());
		}
		else
		{
			const ItemData *c_data = DataCenter::sharedData()->getItemDesc()->getCfg(data.id);
			if(c_data->itemName.size()>0)
				name->setString(c_data->itemName.c_str());
		}
		
		quality->removeAllChildren();
	}
	else if(data.type == 2)
	{
		sprintf(pathIcon, "prop/%d.png", data.thumb);

		const ItemData *c_data = DataCenter::sharedData()->getItemDesc()->getCfg(data.id);
		if(c_data->itemName.size()>0)
			name->setString(c_data->itemName.c_str());
	}

	//添加星级
	if(data.quality>0)
	{
		//添加星星
		CLayout* pStarLayout = getStarLayout(data.quality);
		quality->addChild(pStarLayout);
	}

	if (!icon->initWithFile(pathIcon))
		CCLOG("[ *ERROR ] RecruitResult::setHeroIcon init icon fail %s",pathIcon);

	if (newicon)
		newicon->setVisible(data.isnew);

	quality->setVisible(true);
}

void RecruitResult::turnDispose(bool turn /*=false*/)
{
	CLayout* comLayer = (CLayout*)m_Turn->getChildByTag(commonLayerRes);
	CLayout* getLayer = (CLayout*)m_Turn->getChildByTag(heruTurnLayerRes);
	CLayout* newLayer = (CLayout*)m_Turn->getChildByTag(newHeroLayerRes);

	//结束后的操作界面
	m_Turn->setVisible(true);

	//多个
	if (m_LotteryResult.size() > 1)
	{
		comLayer->setVisible(false);

		//结算按钮界面出现
		if (m_BtnTurn)
		{
			getLayer->setVisible(false);
			newLayer->setVisible(true);
			newLayer->getChildByTag(newButTenLabRes)->setVisible(true);
			newLayer->getChildByTag(newButOenLabRes)->setVisible(false);
			newLayer->getChildByTag(newNewIconRes)->setVisible(false);
			newLayer->getChildByTag(newheartIconRes)->setVisible(false);
			newLayer->getChildByTag(newMijiIconRes)->setVisible(false);
			newLayer->getChildByTag(newTurnLabRes)->setVisible(true);
			newLayer->getChildByTag(newGetLabRes)->setVisible(false);

			CLabel* needNum = (CLabel*)newLayer->getChildByTag(newNumLabRes);
			CButton* buyone = (CButton*)newLayer->getChildByTag(newBuyBtnRes);
			CButton* buysure = (CButton*)newLayer->getChildByTag(newSureBtnRes);
			needNum->setString(ToString(m_needNum));
			roleUpdate(*(TMessage*)(nullptr));


			buyone->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));
			buysure->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));

			m_HeroView->setCountOfCell(m_LotteryResult.size());
			m_HeroView->reloadData();
			m_BtnTurn = false;

		}
		else
		{
			newLayer->setVisible(false);
			getLayer->setVisible(true);
			m_EFView->setVisible(true);
 			
			this->scheduleOnce(schedule_selector(RecruitResult::turnSureRes), m_cellIndex==0?0:2.0f);

			m_HeroView->setCountOfCell(m_LotteryResult.size());
			m_HeroView->reloadData();
			//if (m_LotteryResult.at(0).type == 1 && m_LotteryResult.at(0).isnew)
			//{			
			//	showHeroCall(m_HeroView->cellAtIndex(0));
			//}
		}

	}
	else//一个
	{
		comLayer->setVisible(true);
		//stars->setVisible(true); 
		CLabel* name = (CLabel*)comLayer->getChildByTag(comHeroNameRes);
		CImageView* icon = (CImageView*)comLayer->getChildByTag(comHeroImgRes);
		CCSprite* quality = (CCSprite*)comLayer->getChildByTag(comHeroQuality);
		setHeroIcon(icon,quality,name,m_LotteryResult.at(0));
		
		if(/*m_LotteryResult.at(0).type == 1 && */m_LotteryResult.at(0).isnew)
		{
			m_cellIndex = 0;
			comLayer->setVisible(false);
			newLayer->setVisible(false);
			getLayer->setVisible(true);

			if(m_LotteryResult.at(0).type == 1)
			{
				showNewHeroAnimation();
				runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.5f), CCCallFunc::create(this, callfunc_selector(RecruitResult::showNewHeroAnimationCallBack))));
			}
			else
			{
				showNewHeroAnimationCallBack();
			}
		}
		else if (!m_LotteryResult.at(0).isnew && turn)
		{
			getLayer->setVisible(false);
			newLayer->setVisible(true);
			newLayer->getChildByTag(newButTenLabRes)->setVisible(false);
			newLayer->getChildByTag(newButOenLabRes)->setVisible(true);
			if (m_Type == FriendshipBtnRec)
			{
				newLayer->getChildByTag(newGoldIconRes)->setVisible(false);
				newLayer->getChildByTag(newheartIconRes)->setVisible(true);
			}else{
				newLayer->getChildByTag(newGoldIconRes)->setVisible(true);
				newLayer->getChildByTag(newheartIconRes)->setVisible(false);
			}

			if(m_LotteryResult.at(0).type == 1)
			{
				newLayer->getChildByTag(newMijiIconRes)->setVisible(turn);
			}
			else
			{
				newLayer->getChildByTag(newMijiIconRes)->setVisible(false);
			}
			

			newLayer->getChildByTag(newNewIconRes)->setVisible(!turn);
			newLayer->getChildByTag(newTurnLabRes)->setVisible(turn);
			newLayer->getChildByTag(newGetLabRes)->setVisible(!turn);

			if(m_LotteryResult.at(0).type == 1)
			{
				quality->setVisible(!turn);
			}
			else
			{
				quality->setVisible(true);
			}

			CLabel* needNum = (CLabel*)newLayer->getChildByTag(newNumLabRes);
			needNum->setString(ToString(m_needNum));
			roleUpdate(TMessage());

			CButton* buyone = (CButton*)newLayer->getChildByTag(newBuyBtnRes);
			CButton* buysure = (CButton*)newLayer->getChildByTag(newSureBtnRes);
			buyone->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));
			buysure->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));

			//数量显示
			int iPrizeNum = m_LotteryResult.at(0).m_iNum;
			CLabel *num = (CLabel*)m_Turn->findWidgetById("num_one");
			num->setVisible(false);
			CCLabelAtlas * pLabelNum = (CCLabelAtlas*)quality->getChildByTag(111);
			if(pLabelNum==nullptr)
			{
				CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
				haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
				haveNum->setPosition(ccp(90, 5));
				haveNum->setTag(111);
				quality->addChild(haveNum);
				pLabelNum = haveNum;
			}

			if (iPrizeNum>1)
			{
				pLabelNum->setString(ToString(iPrizeNum));
				pLabelNum->setVisible(true);
			}
			else
			{
				pLabelNum->setVisible(false);
			}

			//添加触摸事件
			icon->setUserData(&m_LotteryResult.at(0));
			icon->setTouchEnabled(true);
			icon->setOnPressListener(this, ccw_press_selector(RecruitResult::onPress));

			if (CGuideManager::getInstance()->getIsRunGuide()&&CGuideManager::getInstance()->getCurrTask()==1)
			{
				CGuideManager::getInstance()->getGuideTaskData()->isFinish = true;
				CGuideManager::getInstance()->nextStep();
			}
		}
		else
		{
			
			getLayer->setVisible(true);
			newLayer->setVisible(false);
			
			m_cellIndex = 1;
			continueMoveLight(0);

			PlayEffectSound(SFX_424);

			CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8001");
			lightAnim->setDelayPerUnit(0.05f);
			CCPoint pos = quality->getPosition();
			pos.y -=5;
			CCSprite *light = CCSprite::create("skill/8001.png");
			light->setPosition(pos);
			light->setScale(0.9f);
			light->runAction(CCSequence::createWithTwoActions(CCAnimate::create(lightAnim),CCHide::create()));
			comLayer->addChild(light);
		}
	}
}

CCObject* RecruitResult::EFviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;
	if (pCell)
	{
		pCell->removeAllChildrenWithCleanup(true);
		EFAddCell(uIdx,pCell);
	}else{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		EFAddCell(uIdx,pCell);
	}
	return pCell;
}

void RecruitResult::EFAddCell(unsigned int uIdx, CGridViewCell* pCell)
{
	char efName [60] = {0};
	sprintf(efName,"1001%d",m_LotteryResult.at(uIdx).quality);
	pCell->setVisible(true);

	int animId = 8033;	
	if (m_LotteryResult.at(uIdx).iColor < 10)
		animId = (m_LotteryResult.at(uIdx).iColor-1)*10 +8003;
	
	CCAnimation *soulAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));
	
	CCSprite *soul = createAnimationSprite(CCString::createWithFormat("skill/%d.png",animId)->getCString(), m_cell1->getChildByTag(11)->getPosition(), soulAnim,true);
	soul->setScale(0.8f);
	soul->setVisible(false);
	soul->setTag(91);
	soul->runAction(CCSequence::create(CCDelayTime::create(0.1*uIdx+0.3), CCShow::create(), nullptr));
	pCell->addChild(soul);
	pCell->runAction(CCCallFuncN::create(this, callfuncN_selector(RecruitResult::cellEffectCallBack)));
}

CCObject* RecruitResult::HeroviewDataSource(CCObject* pConvertCell, unsigned int uIdx)
{
	CGridViewCell* pCell = (CGridViewCell*)pConvertCell;		//只需要使用位置进行处理，不需要cloneView的Cell
	if (pCell)
	{
		pCell->removeAllChildrenWithCleanup(true);
		pCell->setUserData(&m_LotteryResult.at(uIdx));
		HeroAddCell(uIdx,pCell);
	}else{
		pCell = new CGridViewCell();
		pCell->autorelease();
		pCell->setTag(uIdx);
		pCell->setUserData(&m_LotteryResult.at(uIdx));
		HeroAddCell(uIdx,pCell);
	}
	return pCell;
}

void RecruitResult::HeroAddCell(unsigned int uIdx, CGridViewCell* pCell)
{
	CLayout *cell = UICloneMgr::cloneLayout(m_cell2);
	CLabel* name = (CLabel*)cell->getChildByTag(cell_name);
	CImageView* _Img = (CImageView*)cell->getChildByTag(cell_Img);
	CCSprite* _quality = (CCSprite*)cell->getChildByTag(cell_quality);
	CCSprite* _newIcon = (CCSprite*)cell->getChildByTag(cell_newIcon);
	CCSprite* _mijiIcon = (CCSprite*)cell->getChildByTag(cell_mijiIcon);
	CCSprite* _bg = (CCSprite*)cell->getChildByTag(cell_bg);
	
	if (m_BtnTurn)
	{
		setHeroIcon(_Img,_quality,name,m_LotteryResult.at(uIdx));

		//添加触摸事件
		_Img->setUserData(&m_LotteryResult.at(uIdx));
		_Img->setTouchEnabled(true);
		_Img->setOnPressListener(this, ccw_press_selector(RecruitResult::onPress));

		if(m_LotteryResult.at(uIdx).type == 1)
		{
			_mijiIcon->setVisible(!m_LotteryResult.at(uIdx).isnew);
			_quality->setVisible(m_LotteryResult.at(uIdx).isnew);
		}
		else
		{
			_mijiIcon->setVisible(false);
			_quality->setVisible(true);
		}

		_newIcon->setVisible(m_LotteryResult.at(uIdx).isnew);


	}else{
		setHeroIcon(_Img,_quality,name,m_LotteryResult.at(uIdx));
		_mijiIcon->setVisible(false);
		_newIcon->setVisible(m_LotteryResult.at(uIdx).isnew);
		_quality->setVisible(true);
	}

	//数量显示
	int iPrizeNum = m_LotteryResult.at(uIdx).m_iNum;
	CLabel *num = (CLabel*)cell->findWidgetById("num");
	num->setVisible(false);
	CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
	haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
	haveNum->setPosition(ccp(90, 5));
	_quality->addChild(haveNum, 99);
	if (iPrizeNum>1)
	{
		haveNum->setString(ToString(iPrizeNum));
		haveNum->setVisible(true);
	}
	else
	{
		haveNum->setVisible(false);
	}

	//cell->removeChild(num);
	cell->removeChild(_bg);
	cell->removeChild(name);
	cell->removeChild(_Img);
	cell->removeChild(_quality);
	cell->removeChild(_newIcon);
	cell->removeChild(_mijiIcon);

	pCell->addChild(_bg);
	pCell->addChild(name);
	pCell->addChild(_Img);
	pCell->addChild(_quality);
	pCell->addChild(_newIcon);
	pCell->addChild(_mijiIcon);
	//pCell->addChild(num, 99);
 	pCell->setVisible(m_BtnTurn);
}

//英雄头像，黄色特效光
void RecruitResult::showHeroCall(CCNode *pNode)
{
	
	CGridViewCell* pCell = (CGridViewCell*)pNode;
	m_EFView->cellAtIndex(pCell->getIdx())->setVisible(false);
	pNode->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.18f), CCShow::create()));
	if (!m_BtnTurn)
	{
		PlayEffectSound(SFX_424);

		CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("8001");
		lightAnim->setDelayPerUnit(0.05f);
		CCPoint pos = pCell->getChildByTag(26)->getPosition();
		pos.y -=5;
		CCSprite *light = CCSprite::create("skill/8001.png");
		light->setPosition(pos);
		light->setScale(0.9f);
		light->runAction(CCSequence::createWithTwoActions(CCAnimate::create(lightAnim),CCHide::create()));
		pCell->addChild(light);
	}
}

void RecruitResult::netGetRes(float dt)
{
	//成功抽取的label消失掉, 火焰消失掉(如果有的话)
	m_Res->getChildByTag(resTenLabRes)->setVisible(false);
	m_Res->getChildByTag(resOenLabRes)->setVisible(false);
	m_Res->getChildByTag(resImgRes)->setVisible(false);

	turnDispose();
}

void RecruitResult::turnSureRes(float dt)
{
	m_cellIndex =0;
	CCSequence* pAction = CCSequence::createWithTwoActions(CCDelayTime::create(0.1f), CCCallFunc::create(this, callfunc_selector(RecruitResult::heroInfoAnim)));
	pAction->setTag(111);
	this->runAction(pAction);
}

void RecruitResult::heroInfoAnim( )
{
	float delt = 0.0f;
	int j =0;
	for (int i = m_cellIndex; i < m_LotteryResult.size(); i++)
	{
		delt = 0.4*j;
		 if (!m_LotteryResult.at(i).isnew)
		 {
			 j++;
			 CCDelayTime* delay = CCDelayTime::create(delt);
			 CGridViewCell *pCell = m_HeroView->cellAtIndex(i);
			 if (pCell)
			 {		   
				 pCell->runAction(CCSequence::createWithTwoActions(delay,CCCallFuncN::create(this,callfuncN_selector(RecruitResult::showHeroCall))));
			 }
			 m_cellIndex++;
		}
		else
		{	 
			CCDelayTime* delay = CCDelayTime::create(delt);
			CGridViewCell *pCell = m_HeroView->cellAtIndex(i);
			if (pCell)
			{		   
				pCell->runAction(CCSequence::createWithTwoActions(delay,CCCallFuncN::create(this,callfuncN_selector(RecruitResult::showHeroCall))));
			}
			//抽到新英雄动画
			CCSequence* pAction = CCSequence::createWithTwoActions(CCDelayTime::create(delt+3.4f), CCCallFunc::create(this, callfunc_selector(RecruitResult::heroInfoAnim)));
			pAction->setTag(111);
			this->runAction(pAction);
			this->scheduleOnce(schedule_selector(RecruitResult::playNewHeroAnim), delt+0.4f);
			return;
		}
	}
	this->scheduleOnce(schedule_selector(RecruitResult::playNewHeroAnim), delt);
	return;
}

void RecruitResult::continueMoveLight(float dt)
{
	if (m_cellIndex<m_LotteryResult.size())
	{
		m_HeroView->setVisible(true);
		
		m_pNewHeroEffect->setVisible(false);

		CGridViewCell *cell = m_HeroView->cellAtIndex(m_cellIndex);
		showHeroCall(cell);
	}
	else
	{	
		m_pNewHeroEffect->setVisible(false);

		m_HeroView->setVisible(true);
		if (m_LotteryResult.size() > 1)
		{
			m_BtnTurn = true;
			turnDispose();
		}
		else
			turnDispose(true);
	}
}

void RecruitResult::playNewHeroAnim(float dt)
{
	if (m_cellIndex>=m_LotteryResult.size())
	{
		//this->unschedule(schedule_selector(RecruitResult::heroInfoAnim));
		this->stopActionByTag(111);
		this->scheduleOnce(schedule_selector(RecruitResult::continueMoveLight), 0.1f);
		return;
	}

	showNewHeroAnimation();

	m_cellIndex++;
	this->scheduleOnce(schedule_selector(RecruitResult::continueMoveLight), 2.6f);
}

void RecruitResult::showCoffinWithRank()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r1.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r2.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r3.plist");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r1.png");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r2.png");
	CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r3.png");

	switch (m_iLotteryRank)
	{
	case 0://木
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("recruit/r1.plist");
		}break;
	case 1://银
		{
			//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r1.plist");
			//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r3.plist");
			//CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r1.png");
			//CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r3.png");
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("recruit/r2.plist");
		}break;
	case 2://金
		{
			//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r1.plist");
			//CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("recruit/r2.plist");
			//CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r1.png");
			//CCTextureCache::sharedTextureCache()->removeTextureForKey("recruit/r2.png");
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("recruit/r3.plist");
		}break;
	default:
		break;
	}
}

void RecruitResult::showArrowAndButton()
{
	//箭头
	CCNode* pArrow = (CCNode*)m_ui->findWidgetById("arrow");
	pArrow->setVisible(true);
	if(pArrow->getActionByTag(111)==nullptr)
	{
		CCRepeatForever* pRep = CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0, 40)), CCMoveBy::create(0.3f, ccp(0, -40))));
		pRep->setTag(111);
		pArrow->runAction(pRep);
	}
	
	//按钮
	CImageView* pView = (CImageView*)m_ui->findWidgetById("hide_btn");
	pView->setVisible(true);

	if (CGuideManager::getInstance()->getIsRunGuide()&&CGuideManager::getInstance()->getCurrTask()==1)
	{
		CGuideManager::getInstance()->getGuideTaskData()->isFinish = true;
		CGuideManager::getInstance()->nextStep();
	}
}

void RecruitResult::touchCoffin( CCObject* pSender )
{
	//m_armature->getAnimation()->play("expo", 0.01f);
}


void RecruitResult::pressCoffin( CCObject* pSender, CTouchPressState iState )
{
	switch (iState)
	{
	case cocos2d::cocoswidget::CTouchPressOn:
		{
			m_armature->getAnimation()->play("expo", 0.01f);
		}
		break;
	case cocos2d::cocoswidget::CTouchPressOff:
		{
			if(!m_bIsSuccess)
			{
				m_armature->getAnimation()->play("opened", 0.01f);
			}
		}
		break;
	default:
		break;
	}
}

void RecruitResult::showHero()
{
	if (m_LotteryResult.size())
	{
		m_Res->setVisible(false);
		m_Turn->setVisible(false);
		if (m_LotteryResult.size() > 1)
		{
			tensRecruit();
		}
		else
		{
			onesRecruit();
		}

		this->runAction(CCSequence::createWithTwoActions(
			CCDelayTime::create(0.1f*m_LotteryResult.size()+0.4f), 
			CCCallFunc::create(this, callfunc_selector(RecruitResult::callBackForCloesed))));
	}
}

void RecruitResult::checkRank()
{
	//评定等级
	if(m_LotteryResult.size()>1)
	{
		m_iLotteryRank = 2;
	}
	else
	{
		const HeroLotteryData& data = m_LotteryResult.at(0);
		if(data.iColor<=1)
		{
			m_iLotteryRank = 0;
		}
		else if(data.iColor<=3)
		{
			m_iLotteryRank = 1;
		}
		else
		{
			m_iLotteryRank = 2;
		}
	}
}

void RecruitResult::resetArmature()
{
	if(m_armature != nullptr)
	{
		m_armature->removeFromParentAndCleanup(true);
	}
	m_armature = CCArmature::create("zhaomu");
	m_armature->getAnimation()->play("ready", 0.01f);
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.6f), CCCallFunc::create(this, callfunc_selector(RecruitResult::callbackForSound))));
	m_armature->setPosition(VCENTER);
	m_armature->setPositionX(m_armature->getPositionX() - 18);
	m_armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(RecruitResult::openMovementEvent));
	m_armature->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(RecruitResult::openFrameEvent));

	this->addChild(m_armature,-1);
	//箭头
	CCNode* pArrow = (CCNode*)m_ui->findWidgetById("arrow");
	pArrow->setVisible(false);
	CImageView* pView = (CImageView*)m_ui->findWidgetById("hide_btn");
	pView->setVisible(false);

	PlayEffectSound(SFX_421);
	m_bIsSuccess = false;
	m_cellIndex = 0;
}

void RecruitResult::cellEffectCallBack( CCNode* pSender )
{
	CCNode* pCell = (CCNode*)pSender;
	int uIdx = pCell->getTag();

	int animId = 8033;	
	if (m_LotteryResult.at(uIdx).iColor < 10)
		animId = (m_LotteryResult.at(uIdx).iColor-1)*10 +8003;

	CCAnimation *soulAnim = AnimationManager::sharedAction()->getAnimation(ToString(animId));

	CCSprite *soulMove = createAnimationSprite(CCString::createWithFormat("skill/%d.png", animId)->getCString(), ccp(1138/2, 250), soulAnim, true);
	soulMove->setScale(0.2f);
	CCPoint pos = pCell->convertToWorldSpace(pCell->getChildByTag(91)->getPosition());
	CCMoveTo *move = CCMoveTo::create(0.25, pos);
	soulMove->setVisible(false);
	soulMove->runAction(CCSequence::create(CCDelayTime::create(0.1*uIdx), CCShow::create(), 
		CCCallFunc::create(this, callfunc_selector(RecruitResult::flySoundCallBack)),
		CCSpawn::createWithTwoActions(CCEaseBackInOut::create(move), CCScaleTo::create(0.3f, 0.8f)), CCRemoveSelf::create(),nullptr));
	m_ui->addChild(soulMove);
}

void RecruitResult::showNewHeroAnimation()
{
	m_HeroView->setVisible(false);
	HeroLotteryData *lottery = &m_LotteryResult.at(m_cellIndex);
	m_pNewHeroEffect->showNewHeroEffect(lottery);
}

void RecruitResult::showNewHeroAnimationCallBack()
{
	m_pNewHeroEffect->setVisible(false);

	CLayout* comLayer = (CLayout*)m_Turn->getChildByTag(commonLayerRes);
	CLayout* getLayer = (CLayout*)m_Turn->getChildByTag(heruTurnLayerRes);
	CLayout* newLayer = (CLayout*)m_Turn->getChildByTag(newHeroLayerRes);

	CLabel* name = (CLabel*)comLayer->getChildByTag(comHeroNameRes);
	CImageView* icon = (CImageView*)comLayer->getChildByTag(comHeroImgRes);
	CCSprite* quality = (CCSprite*)comLayer->getChildByTag(comHeroQuality);

	comLayer->setVisible(true);
	newLayer->setVisible(true);
	getLayer->setVisible(false);

	newLayer->getChildByTag(newButTenLabRes)->setVisible(false);
	newLayer->getChildByTag(newButOenLabRes)->setVisible(true);
	if (m_Type == FriendshipBtnRec)
	{
		newLayer->getChildByTag(newGoldIconRes)->setVisible(false);
		newLayer->getChildByTag(newheartIconRes)->setVisible(true);
	}else{
		newLayer->getChildByTag(newGoldIconRes)->setVisible(true);
		newLayer->getChildByTag(newheartIconRes)->setVisible(false);
	}
	newLayer->getChildByTag(newMijiIconRes)->setVisible(false);
	newLayer->getChildByTag(newNewIconRes)->setVisible(true);
	newLayer->getChildByTag(newTurnLabRes)->setVisible(false);
	newLayer->getChildByTag(newGetLabRes)->setVisible(true);
	quality->setVisible(true);
	CLabel* needNum = (CLabel*)newLayer->getChildByTag(newNumLabRes);
	needNum->setString(ToString(m_needNum));
	roleUpdate(TMessage());

	CButton* buyone = (CButton*)newLayer->getChildByTag(newBuyBtnRes);
	CButton* buysure = (CButton*)newLayer->getChildByTag(newSureBtnRes);
	buyone->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));
	buysure->setOnClickListener(this,ccw_click_selector(RecruitResult::ResBtnCallBack));

	//数量显示
	int iPrizeNum = m_LotteryResult.at(0).m_iNum;
	CLabel *num = (CLabel*)m_Turn->findWidgetById("num_one");
	num->setVisible(false);
	CCLabelAtlas * pLabelNum = (CCLabelAtlas*)quality->getChildByTag(111);
	if(pLabelNum==nullptr)
	{
		CCLabelAtlas* haveNum = CCLabelAtlas::create("", "label/no_02.png", 9, 15, 46);
		haveNum->setAnchorPoint(ccp(1.0f, 0.0f));
		haveNum->setPosition(ccp(90, 5));
		haveNum->setTag(111);
		quality->addChild(haveNum);
		pLabelNum = haveNum;
	}

	if (iPrizeNum>1)
	{
		pLabelNum->setString(ToString(iPrizeNum));
		pLabelNum->setVisible(true);
	}
	else
	{
		pLabelNum->setVisible(false);
	}

	//添加触摸事件
	icon->setUserData(&m_LotteryResult.at(0));
	icon->setTouchEnabled(true);
	icon->setOnPressListener(this, ccw_press_selector(RecruitResult::onPress));

	if (CGuideManager::getInstance()->getIsRunGuide()&&CGuideManager::getInstance()->getCurrTask()==1)
	{
		CGuideManager::getInstance()->getGuideTaskData()->isFinish = true;
		CGuideManager::getInstance()->nextStep();
	}
}

void RecruitResult::showHeroCardCallBack()
{

}

void RecruitResult::flySoundCallBack()
{
	PlayEffectSound(SFX_422);
}

void RecruitResult::onPress( CCObject* pSender, CTouchPressState iState )
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
				m_pMonsterInfo->changeForSign(&pHero);
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
				prize.num = LotteryData->m_iNum;
				prize.quality = LotteryData->quality;
				prize.color = LotteryData->iColor;
				m_pItemInfo->setInfo(&prize);
				m_pItemInfo->bindPos(pImage);
				m_pItemInfo->changeForSign(&prize);
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

void RecruitResult::roleUpdate( const TMessage& tMsg )
{
	CLayout* newLayer = (CLayout*)m_Turn->getChildByTag(newHeroLayerRes);
	CLabel* needNum = (CLabel*)newLayer->getChildByTag(newNumLabRes);
	needNum->setString(ToString(m_needNum));
	if (m_Type == FriendshipBtnRec)
	{
		//检查货币数量
		if(!CheckFriendShip(atoi(needNum->getString())))
		{
			needNum->setColor(RGB_RED);
		}
		else
		{
			needNum->setColor(RGB_WHITE);
		}
	}else{
		//检查货币数量
		if(!CheckGold(atoi(needNum->getString())))
		{
			needNum->setColor(RGB_RED);
		}
		else
		{
			needNum->setColor(RGB_WHITE);
		}
	}
}

void RecruitResult::callbackForSound()
{
	PlayEffectSound(SFX_540);
}

void RecruitResult::callBackForCloesed()
{
	m_armature->getAnimation()->play("closed", 0.01f);
}


