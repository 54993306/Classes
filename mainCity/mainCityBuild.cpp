#include "mainCityBuild.h"
#include "Global.h"
#include "GoldMineLayer.h"
#include "MainCityControl.h"
#include "CCompaseLayer.h"
#include "CultivateLayer.h"

#include "Strengthen.h"
#include "Recruit.h"

#include "CommandLayer.h"
#include "Battle/AnimationManager.h"
#include "netcontrol/CPlayerControl.h"
#include "tools/ShowTexttip.h"
#include "guide/GuideManager.h"
#include "TopLayer.h"
#include "GMessage.h"
#include "reward/RewardLayer.h"
#include "shop/ShopLayer.h"
#include "task/TaskControl.h"

#include "Battle/BattleScene/LoadBattleResource.h"
#include "common/CGameSound.h"
#include "common/CommonFunction.h"
#include "rank/RankLayer.h"

#include "reward/WorldBoss.h"
#include "pvp_ui/PvpRecord.h"

#include "pvp_ui/PvpGateLayer.h"

#include "SDK/GamePlatformManager.h"
#include "mainCity/mainScene.h"
#include "tools/CCShake.h"

using namespace BattleSpace;

bool CMainCityBuild::init()
{
	BaseLayer::init();

	setVisible(true);
	setTouchPriority(0);

	CWidgetWindow* layout = CWidgetWindow::create();
	layout->setTag(10000);
	layout->setTouchSwallow(false);
	layout->setTouchPriority(-1);
	addChild(layout);
	//主城地图加载
	m_ui = LoadComponent("MainBuild.xaml");
	m_ui->setPosition(VCENTER);
	layout->addChild(m_ui);

	LayerManager::instance()->setPriority(-2);
	m_cityUi = CMainCityUI::create();
	m_cityUi->setTouchPriority(-2);
	this->addChild(m_cityUi,0,20010);
// 	LayerManager::instance()->push(m_cityUi);

	show(nullptr);

	m_frontLay = (CLayout*)m_ui->findWidgetById("frontLayer");
	m_bgLay =(CLayout*)m_ui->findWidgetById("bgLayer");

	m_fMoveSpeed = 0.0f;

	return true;
}

void CMainCityBuild::onEnter()
{	
	CCSize deviceSize = CCDirector::sharedDirector()->getWinSizeInPixels();

	m_minX = WINSIZE.width/2;
	m_maxX = 2616-WINSIZE.width/2;

	BaseLayer::onEnter();

 	this->scheduleUpdate();

	cityHeroMovePoint();
	for (int i=1; i<=17; ++i)
	{
		CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(i));
		if (btn)
		{
			btn->setOnClickListener(this,ccw_click_selector(CMainCityBuild::onClickButton));
			m_buildPos[i-1] = ccp(btn->getPositionX(),btn->getPositionY() - btn->boundingBox().size.height/2);
		}
	}

 	CMainCityControl::getInstance()->sendEnterCity();
	//绑定场景隐藏和显示的消息
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityBuild::show), SHOW_MAIN_SCENE, nullptr);
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityBuild::hide), HIDE_MAIN_SCENE, nullptr);
	
	NOTIFICATION->addObserver(this, callfuncO_selector(CMainCityBuild::backFromPvp), "CMainCityBuild::backFromPvp", nullptr);

	PlayBackgroundMusic(BGM_MainCity,true);

	//不再需要走路的英雄——又需要了
	CSceneManager::sharedSceneManager()->addMsgObserver(GARRSION_HERO,this,GameMsghandler_selector(CMainCityBuild::updateGarrsionHero));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_BUILD,this,GameMsghandler_selector(CMainCityBuild::updateCityBuild));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_HERO,this,GameMsghandler_selector(CMainCityBuild::updateMobileShop));

	GetTcpNet->registerMsgHandler(ActListMsg, this, CMsgHandler_selector(CMainCityBuild::recvWorldBossNotice));

	//createBuildEffect();
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
	
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("skill/shangchengbaobao0.png","skill/shangchengbaobao0.plist","skill/shangchengbaobao.ExportJson");
	m_armature = CCArmature::create("shangchengbaobao");
	m_armature->getAnimation()->play("tiao",-1,-1,0);
	m_armature->setPosition(ccp(80,100));
	m_armature->setScaleX(-1);
	m_frontLay->addChild(m_armature);
	m_armature->getAnimation()->setMovementEventCallFunc(this,movementEvent_selector(CMainCityBuild::moveEvent));
	m_armature->getAnimation()->setFrameEventCallFunc(this,frameEvent_selector(CMainCityBuild::openFrameEvent));
	m_armature->setVisible(user->getMobileShop());

	if (user->getMobileShop())
	{	
		if (user->getFirstMobileShop())
		{		
			m_frontLay->setPositionX(m_maxX);
			m_bgLay->setPositionX(m_iBgLayerMaxLeftBasePos);
			user->setFirstMobileShop(false);
		}
	}

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
 	//user->setNewStep(1);
	this->scheduleOnce(schedule_selector(CMainCityBuild::runGuideStep),0.4f);
#else
	this->scheduleOnce(schedule_selector(CMainCityBuild::runGuideStep),0.4f);
#endif

	//正在引导，不自动弹窗
	if(user->getNewStep()>0 && user->getNewStep()!=100)
	{
		DataCenter::sharedData()->setCityActionType(CA_None);
	}

	CButton *btn = NULL;
	switch(DataCenter::sharedData()->getCityActionType())
	{
	case CA_GetHero:
		btn = (CButton*)m_frontLay->getChildByTag(Btn_Recruit);	
		break;
	case CA_BlackSmith:
		if (CMainCityControl::getInstance()->isCityOpen(4))
		{
			btn = (CButton*)m_frontLay->getChildByTag(Btn_Strengthen);	
		}
		break;
	case CA_Combine:
		btn = (CButton*)m_frontLay->getChildByTag(Btn_CompaseTa);	
		break;			
	case CA_GoToReward:
		btn = (CButton*)m_frontLay->getChildByTag(Btn_School);	
		break;	
	case CA_GoToWorldBoss:
		btn = (CButton*)m_frontLay->getChildByTag(Btn_Monon);	
		break;	
	}

	if (btn != NULL)
	{
		btn->runAction(CCSequence::create(CCDelayTime::create(0.3f),
			CCCallFuncN::create(this, callfuncN_selector(CMainCityBuild::onTimeWaitCityAction)),
			NULL));
		DataCenter::sharedData()->setCityActionType(CA_None);

	}

	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_FUNCTIONOPEN,this,GameMsghandler_selector(CMainCityBuild::updateFuctionOpen));
	CSceneManager::sharedSceneManager()->addMsgObserver(UPDATE_GAMETIP,this,GameMsghandler_selector(CMainCityBuild::updateGameTip));
	showNoticeTip(CTaskControl::getInstance()->getGameTips());

	//显示平台悬浮窗
	GamePlatformMgr->ShowPlatformFloatWindow();

	//弹出PVP界面
	if(DataCenter::sharedData()->getCityActionType() == CA_PVP)
	{
		//自动上天
		runAction(CCSequence::createWithTwoActions(
			CCDelayTime::create(0.1f),
			CCCallFunc::create(this, callfunc_selector(CMainCityBuild::autoToPvp))
			));
		DataCenter::sharedData()->setCityActionType(CA_None);
	}

}

void CMainCityBuild::resetPos(CCNode* node)
{
	node->setPosition(ccp(-500,VCENTER.y));
}

void CMainCityBuild::removeMobieShop(float dt)
{
	if (m_armature)
	{
		m_armature->removeFromParent();
		m_armature = nullptr;
	}
}

void CMainCityBuild::moveEvent(CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	if (movementType==COMPLETE&&strcmp(movementID,"tiao")==0)
	{
		m_armature->getAnimation()->play("xunhuan",-1,-1,1);
	}
	else if (movementType==COMPLETE&&strcmp(movementID,"baozha")==0)
	{
		//m_armature->getAnimation()->play("tiao");
		if( LayerManager::instance()->getLayer("CShopLayer") == nullptr )
		{
			CShopLayer *shpLayer = CShopLayer::create();
			shpLayer->setShopType(2);
			LayerManager::instance()->push(shpLayer);
			CMainCityControl::getInstance()->sendShopRequest(2);
		}
		m_armature->getAnimation()->play("xunhuan",-1,-1,1);
	}
} 

void CMainCityBuild::openFrameEvent(CCBone *bone, const char *name, int orginIndex, int currendtIndex)
{

}

void CMainCityBuild::runGuideStep(float dt)
{
	UserData *user = DataCenter::sharedData()->getUser()->getUserData();
//	user->setNewStep(1);
	int step = user->getNewStep();
	if (step>0/*&&step!=4*/&&step!=9&&step!=12)
	{
		CGuideManager::getInstance()->setTaskStep(user->getNewStep(),1);
		CGuideManager::getInstance()->enterGuide();
	}
	CGuideManager::getInstance()->setJumpStep(false);
}

void CMainCityBuild::updateGarrsionHero(const TMessage& tMsg)
{
	int cityId= tMsg.nMsg;
	CHero *hero =(CHero*)tMsg.lParam;
	//移除
	if (!hero)
	{
		m_frontLay->removeChildByTag(200+cityId);
		//尝试重新找一个放置，没有就算了
		CHero* pHero = CMainCityControl::getInstance()->getGarrsionHeroIdByCityId(cityId);
		if(pHero!=nullptr)
		{
			updateCityGarrsion(pHero,cityId);
		}
	}
	else
	{
		m_frontLay->removeChildByTag(200+cityId);
		updateCityGarrsion(hero,cityId);
		delete hero;
	}
}


void CMainCityBuild::onClickButton(CCObject* pSender)
{
	if(m_bTouchLock) return;

	CButton *btn = dynamic_cast<CButton*>(pSender);
	map<int,CCity>::iterator iter = m_cityMap.find(btn->getTag());

	if (iter!=m_cityMap.end()&&!iter->second.isOpen/* && btn->getTag() != 17*/)
	{
		string str = GETLANGSTR(iter->second.cityId*1000);
		if (str=="")
		{
			str = GETLANGSTR(2000);
		}
		ShowPopTextTip(str.c_str());
		return;
	}

	switch (btn->getTag())
	{
		//瞭望塔
	case		Btn_CompaseTa:
		{
// 			LayerManager::instance()->setPriority(-5);
			CCompaseLayer *tower = CCompaseLayer::create();
			LayerManager::instance()->push(tower);
			CPlayerControl::getInstance().sendComposeItem(1);
		}
		break;
		//金矿
	case		Btn_Arena:
		{
// 			CGlodMineLayer *goldLayer = CGlodMineLayer::create();
// 			LayerManager::instance()->push(goldLayer);
// 			CMainCityControl::getInstance()->sendCityInfo(Btn_Jinkuang/*m_cityMap[Btn_Jinkuang].id*/);
			//ShowPopTextTip(GETLANGSTR(202));

			////进入竞技场
			//CPvpGateLayer* pPvpGate = CPvpGateLayer::create();
			//pPvpGate->onOpen();

		}
		break;
		//强化
	case		Btn_Strengthen:
		{
			CStrengthen *strengthen = CStrengthen::create();
			LayerManager::instance()->push(strengthen);
			hideTips(Btn_Strengthen);
			strengthen->showRedTip();		
			CTaskControl::getInstance()->getGameTips()->strenTips =false;
			CTaskControl::getInstance()->getGameTips()->risingTips =false;
		}
		break;
		//培植区
	case		Btn_Cultivate:
		{
// 			LayerManager::instance()->setPriority(-5);
			CCultivateLayer *cultivateLayer = CCultivateLayer::create();
			LayerManager::instance()->push(cultivateLayer);
			CMainCityControl::getInstance()->sendCityInfo(6);
			hideTips(Btn_Cultivate);
			CTaskControl::getInstance()->getGameTips()->farmTips =false;
		}
		break;
		//野矿
	case		Btn_Yekuang:

		break;
		//祭坛
	case		Btn_Jituan:
		
		break;
		//指挥府
	case		Btn_Zhihuifu:
		{
// 			CCommandLayer *layer = CCommandLayer::create();
// 			LayerManager::instance()->push(layer);
// // 			CMainCityControl::getInstance()->sendCityInfo(Btn_Zhihuifu);
// 			GetTcpNet->sendDataType(CommanderMsg);
			ShowPopTextTip(GETLANGSTR(202));
		}
		break;
		//兵营
	case		Btn_Binying:
		{

		}
		break;
		//铁锵铺
	case		Btn_Recruit:
		{
			CRecruit *recr = CRecruit::create();
			recr->showRedTip(CTaskControl::getInstance()->getGameTips()->lotteryTips);
			LayerManager::instance()->push(recr);
			hideTips(Btn_Recruit);
			CTaskControl::getInstance()->getGameTips()->lotteryTips =false;
// 			CRankLayer *rank = CRankLayer::create();
// 			LayerManager::instance()->push(rank);
// 			CPlayerControl::getInstance().sendRank(1);
		}
		break;
		//杂货店
	case		Btn_Zahuodian:

		break;
		//农场
	case		Btn_Nongchang:

		break;
		//竞技场
	case		Btn_jingjichang:
		
		break;
	case	Btn_Rank:
		{
			//ShowPopTextTip(GETLANGSTR(202));
 			CRankLayer *rank = CRankLayer::create();
// 			CPvpRecord *rank = CPvpRecord::create();
			LayerManager::instance()->push(rank);
			CPlayerControl::getInstance().sendRank(2);
		}
		break;
	case	Btn_School:
		{
			CRewardLayer *reward = CRewardLayer::create();
		    LayerManager::instance()->push(reward);
			GetTcpNet->sendDataType(BountyDataMsg,true);
			hideTips(Btn_School);
			CTaskControl::getInstance()->getGameTips()->bountyTips =false;
		}
		break;
	case   Btn_Monon:
		{
			CCNode *ziti = (CCNode*)(m_frontLay->findWidgetById("ziti"));
			CCSprite *red = (CCSprite*)ziti->getChildByTag(1603);
			red->setVisible(false);

			CWorldBoss* pLayer = CWorldBoss::create();
			LayerManager::instance()->push(pLayer);

		}break;
	case   Btn_Tower:
		{
			//ShowPopTextTip(GETLANGSTR(202));

			//Pvp进入
			showPvp();

		}
		break;
	default:
		break;
	}
}

void CMainCityBuild::onEnterTransitionDidFinish()
{
	m_isLoadFinish = true;

	m_foodPos = m_frontLay->getChildByTag(50)->getPosition();
	m_coinPos = m_frontLay->getChildByTag(51)->getPosition();

	m_fogLay = (CLayout *)m_ui->findWidgetById("fog2");
	m_fogLay1 =(CLayout*)m_ui->findWidgetById("fog3");

	/*m_fogLay->setPosition(VCENTER);*/
/*	this->getChildByTag(1)->addChild(m_fogLay);*/
	this->runCloudAction();

	this->runFogAction();
}

void CMainCityBuild::blinkShow(CCNode *node)
{
	node->setVisible(true);
}

void CMainCityBuild::showCityInfo(CityData& cityData)
{
	m_cityUi->updateOpenState(&cityData);

	for (int i=1; i<=16; ++i)
	{
		/*CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(i));*/
		SndButton *btn = (SndButton*)(m_frontLay->getChildByTag(i));
		if (btn)
		{
// 			btn->setVisible(false);
			btn->setPlaySound(true);
		}
	}

	for (int i = 0; i < cityData.cityList.size(); i++)
	{
		CCity *cb = &(cityData.cityList[i]);
		CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(cb->cityId));
		if (cb->heroList.size()>0)
		{
			addMoveHero(cb, i);
		}
		bool justOpen = CMainCityControl::getInstance()->isJustOpen(cb);
		if ((!cb->isOpen&&btn)||(justOpen&&btn))
		{
// 			btn->setEnabled(false);
			CLayout *zitiLay = dynamic_cast<CLayout*>(m_frontLay->findWidgetById("ziti"));
			CCNode *font = zitiLay->getChildByTag(cb->cityId*100+1);
			font->setVisible(false);
			CCNode *di = zitiLay->getChildByTag(cb->cityId*100+2);
			di->setVisible(false);
		}
		if (btn&&!cb->isOpen)
		{
			btn->setNormalTexture(((CCSprite*)btn->getDisabledImage())->getTexture());
		}
		if (justOpen&&btn)
		{
			btn->setEnabled(false);
		}
		m_cityMap[cb->cityId] = *cb;
		if (cb->cityId==16&&!cb->isOpen)
		{
			//mainCity/Build/Building_12_disable.png
			btn->setNormalImage("mainCity/Build/Building_12_disable.png");
		}
// 		updateBuildTexture(cb);
	}

	createBuildEffect();
}

void CMainCityBuild::update(float delta)
{
// 	CCPoint heroPos = m_hero[0]->getPosition();
// // 	
// 	if (m_movePos.getLength() > 0.1f)
// 	{
// 		float speed = 40.0f;
// 		CCPoint nextPos = ccp(heroPos.x + delta * speed *m_real.x, heroPos.y + delta * speed*m_real.y);
// 		m_hero[0]->setPosition(nextPos);
// 		if(ccpDistance(m_movePos, nextPos) < 10)
// 		{
// 			CCArmature*at =(CCArmature*)m_hero[0]->getChildByTag(100);
// 			at->getAnimation()->play("standby");
// 			float delayTime =  random_range(3,8);
// 			this->scheduleOnce(schedule_selector(CMainCityBuild::moveHero),delayTime);
// 			this->unscheduleUpdate();
// 		}else
// 		{
// 			CCArmature*at =(CCArmature*)m_hero[0]->getChildByTag(100);
// 			at->getAnimation()->play("move");
// 		}
// 	}
	m_fmoveTime += delta;
	if(fabs(m_fMoveSpeed) < 100)
	{
		return ;
	}
	float mx = m_frontLay->getPositionX();
	float bx = m_bgLay->getPositionX();

	mx += m_fMoveSpeed * 0.01f;
	bx += (m_fMoveSpeed * 0.01f) * 0.7f;
	if (mx < m_minX)
	{
		mx = m_minX;
		m_fMoveSpeed = 0;
	}
	else if (mx>m_maxX)
	{
		mx = m_maxX;
		m_fMoveSpeed = 0;
	}
	if(bx < 792)
		bx = 792;
	else if(bx > 1822)
		bx = 1822;
	m_frontLay->setPositionX(mx);
	m_bgLay->setPositionX(bx);
	m_fMoveSpeed -= m_fMoveSpeed * 0.04f;
}

void CMainCityBuild::moveHero(float delta)
{
	CCPoint heroPos = m_hero[0]->getPosition();
	
	int indexPos = random_range(0,2);
	m_movePos = m_movePoint[indexPos];
	m_real = ccpNormalize(ccpSub(m_movePos, heroPos));
	if(m_movePos.x - m_hero[0]->getPositionX()>0)
	{
		m_hero[0]->getChildByTag(100)->setScaleX(1);
	}else
	{
		m_hero[0]->getChildByTag(100)->setScaleX(-1);
	}
	CCArmature*at =(CCArmature*)m_hero[0]->getChildByTag(100);
	at->getAnimation()->play("move", 0.01f);
	this->unschedule(schedule_selector(CMainCityBuild::moveHero));

	float distance = ccpDistance(m_movePos,m_hero[0]->getPosition());

	CCMoveTo *move = CCMoveTo::create(distance*0.02,m_movePos);
	CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(CMainCityBuild::walkStop));
	m_hero[0]->runAction(CCSequence::createWithTwoActions(move,call));

}

void CMainCityBuild::walkStop(CCNode *node)
{
	node->stopAllActions();
	CCArmature*at =(CCArmature*)node->getChildByTag(100);
	at->getAnimation()->play("standby", 0.01f);
	/*m_hero[0]->setActionKey(SKELETON_STAND);*/
	float delayTime =  random_range(3,5);
	CCDelayTime *delay = CCDelayTime::create(delayTime);
	CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(CMainCityBuild::repeatMove));
	node->runAction(CCSequence::create(delay,call,nullptr));
}

void CMainCityBuild::repeatMove(CCNode *node)
{
	int indexPos = random_range(0,2);
	//	CCPoint *pos = (CCPoint*)m_hero[i]->getUserData();
	/*m_movePoint[indexPos];*/

	int tag = node->getTag();

	CCPoint *pos;

	if (m_garrsionPointMap.find(tag-200)!=m_garrsionPointMap.end())
	{
		pos = m_garrsionPointMap[tag-200];
	}
	else
	{
		pos = m_garrsionPointMap[12];
	}

    m_movePos = pos[indexPos];

	CCArmature *at = (CCArmature*)node->getChildByTag(100);
	if(m_movePos.x -node->getPositionX()>0)
	{
		node->getChildByTag(100)->setScaleX(1);
	}else
	{
		node->getChildByTag(100)->setScaleX(-1);
	}
	at->getAnimation()->play("move");

	float distance = ccpDistance(m_movePos,node->getPosition());
	CCMoveTo *move = CCMoveTo::create(distance*0.02,m_movePos);
	CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(CMainCityBuild::walkStop));
	node->runAction(CCSequence::createWithTwoActions(move,call));
}

void CMainCityBuild::onExit()
{
	GameSound->stopMusic();
	BaseLayer::onExit();

	CSceneManager::sharedSceneManager()->removeMsgObserver(GARRSION_HERO,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_BUILD,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_HERO,this);

	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_FUNCTIONOPEN,this);
	CSceneManager::sharedSceneManager()->removeMsgObserver(UPDATE_GAMETIP,this);

	//解绑网络回调处理
	GetTcpNet->unRegisterAllMsgHandler(this);

	NOTIFICATION->removeAllObservers(this);

	//隐藏平台悬浮窗
	GamePlatformMgr->HidePlatformFloatWindow();

}
 
bool CMainCityBuild::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if(m_bTouchLock)
	{
		return false;
	}

	m_preMovePos = m_ui->convertToNodeSpace(pTouch->getLocation());
	m_beginPos = m_preMovePos;
	m_fMoveSpeed = 0.0f;
	m_fmoveTime = 0.0f;
	
	m_isInterTouch = false;
	CCRect rect(0,120,100,150);
	if (m_armature&&m_armature->isVisible()&&m_armature->boundingBox().containsPoint(m_frontLay->convertToNodeSpace(pTouch->getLocation())))
	{
		m_isTouchShop =  true;
	}

	return true;
}

void CMainCityBuild::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint locPos = m_ui->convertToNodeSpace(pTouch->getLocation());

	{
		float distance = locPos.x - m_preMovePos.x;
		float posX = m_frontLay->getPositionX() + distance;
		float bgX = m_bgLay->getPositionX() + distance*0.7;
		if (posX<m_minX)
		{
			posX = m_minX;
		}
		else if (posX>m_maxX)
		{
			posX = m_maxX;
		}
		if (bgX<m_minX)
		{
			bgX = m_minX;
		}
		else if (bgX>m_maxX)
		{
			bgX = m_maxX;
		}
		m_preMovePos.x = locPos.x;
		m_frontLay->setPositionX(posX);
		if (posX!=m_maxX&&posX!=m_minX)
		{	
			m_bgLay->setPositionX(bgX);
		}
		//float x = -pos.x + pos.x * point->getRatio().x + point->getOffset().x;  
	}	
	
	if ((fabs(locPos.x - m_beginPos.x)>20.f||fabs(locPos.y - m_beginPos.y)>20.0f)&&!m_isInterTouch)
	{
		for (int i=1; i<=16; ++i)
		{
			CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(i));
			if (btn)
			{ 
				btn->interruptTouch(pTouch,1.0f);
			}
		}
		m_isInterTouch = true;
	}
}

void CMainCityBuild::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint movePos = m_ui->convertToNodeSpace(pTouch->getLocation());
// 	CCPoint distance = ccpSub(movePos,m_preMovePos);
// 	distance.y = 0;
// 	distance.x = distance.x*2;
// 	CCPoint pos = ccpAdd(m_ui->getPosition(),distance);
// 	if (pos.x< m_minX)
// 	{
// 		pos.x = m_minX;
// 	}
// 	else if (pos.x>m_maxX)
// 	{
// 		pos.x = m_maxX;
// 	}
// 	CCMoveTo *move = CCMoveTo::create(0.3f,pos);
// 	//m_ui->setPosition(pos);
// 	m_ui->runAction(move);

// 	if (fabs(movePos.x - m_beginPos.x)>20.f||fabs(movePos.y - m_beginPos.y)>20.0f)
// 	{
// 		for (int i=1; i<=12; ++i)
// 		{
// 			CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(i));
// 			if (btn)
// 			{
// 				btn->interruptTouch(pTouch,1.0f);
// 			}
// 		}
// 	}
	float distance = movePos.x - m_beginPos.x;
	float foffsetX = distance*100 + movePos.x;
	//float boffsetX = distance*100 *0.8 + m_bgLay->getPositionX();

	if (foffsetX<m_minX)
	{
		foffsetX = m_minX;
	}
	else if (foffsetX>m_maxX)
	{
		foffsetX = m_maxX;
	}
	
	if(m_fmoveTime == 0.0f){
		m_fMoveSpeed = 0.0f;
	}else{
		m_fMoveSpeed = distance / m_fmoveTime;
	}
//	m_bgLay->runAction(CCMoveTo::create(0.3f,ccp(boffsetX,m_bgLay->getPositionY())));
//	m_frontLay->runAction(CCMoveTo::create(0.3f,ccp(foffsetX,m_frontLay->getPositionY())));

	if (m_isTouchShop&&m_armature->boundingBox().containsPoint(m_frontLay->convertToNodeSpace(pTouch->getLocation())))
	{
		CCLOG("sg");
		if(m_armature->getAnimation()->getCurrentMovementID() != "baozha")
		{
			m_armature->getAnimation()->play("baozha");
		}
		m_isTouchShop =  false;
	}
}

void CMainCityBuild::ccTouchCancelled( CCTouch *pTouch, CCEvent *pEvent )
{
	
}

void CMainCityBuild::onGetButton(CCObject* pSender)
{
	int mul = 1;
	CButton *btn = (CButton*)pSender;

	btn->getParent()->setVisible(false);

	CCPoint offset = ccpSub(VCENTER,m_ui->getPosition());

	for (int i = 0; i < 5; i++)
	{
		float randX = -100 + 50*i;
		float randY = -150* CCRANDOM_0_1();
		float rver = random_range(280,320);
		CCSprite *spr =nullptr;

		CCMoveTo *moveTo = nullptr;
		//粮食
		if (btn->getTag()==1)
		{ 
			spr = CCSprite::create("mainCity/tili.png");
			CMainCityControl::getInstance()->sendGetCityResource(m_cityMap[6].id);
			moveTo = CCMoveTo::create(0.5f,ccpAdd(m_foodPos,offset));
		}
		//金钱
		else if (btn->getTag()==2)
		{
			spr = CCSprite::create("mainCity/icon_11.png");
			CMainCityControl::getInstance()->sendGetCityResource(m_cityMap[3].id);
			moveTo = CCMoveTo::create(0.5f,ccpAdd(m_coinPos,offset));
		}
		spr->setPosition(btn->getPosition());
		ccBezierConfig cfg;
		cfg.controlPoint_1 = ccp(randX, randY+100);
		cfg.controlPoint_2 = ccp(randX*1.5,randY-random_range(50,100));
		cfg.endPosition = ccp(randX*1.2,-rver);
		CCActionInterval* bezier = CCBezierBy::create(0.6f, cfg);
		CCMoveBy *move = CCMoveBy::create(0.5f,ccp(0,30.0f));
		CCSequence *seque = CCSequence::create(bezier,move,move->reverse(),nullptr);
		spr->runAction(CCSpeed::create(seque,2.0f));

		CCDelayTime *delay = CCDelayTime::create(1.6f);

		
// 		CCMoveBy *moveBy = CCMoveBy::create(0.5f,subPos);
		CCEaseInOut *ease = CCEaseInOut::create(moveTo,2.0f);
		CCCallFuncN *call = CCCallFuncN::create(this,callfuncN_selector(CMainCityBuild::removeCoin));
		CCSequence *seq = CCSequence::create(delay,ease,call,nullptr);
		spr->runAction(seq);
		m_ui->addChild(spr);
	}
}

void CMainCityBuild::removeCoin(CCNode * node)
{
	node->removeFromParent();
}

void CMainCityBuild::runShake(CCNode * lay)
{
	CCMoveBy *down = CCMoveBy::create(0.65f,ccp(0,-15));
	CCMoveBy *up = CCMoveBy::create(0.65f,ccp(0,15));
	CCSequence *seque = CCSequence::createWithTwoActions(down,up);
	lay->runAction((CCSpeed::create(CCRepeatForever::create(seque),2.0f)));
}

void CMainCityBuild::updateCityCap(CCity &city, CLayout * lay, CButton * btn)
{
	if (city.capFull)
	{
		lay->setVisible(true);
		btn->setOnClickListener(this, ccw_click_selector(CMainCityBuild::onGetButton));
		runShake(lay);
	}
	else
	{
		lay->setVisible(false);
	}
}

void CMainCityBuild::runCloudAction()
{
	//云运动
// 	CCSprite *cloud  =  (CCSprite *)(m_fogLay->findWidgetById("cloud"));

	float width = 2616;//cloud->getContentSize().width;

	CCMoveBy* move = CCMoveBy::create(60.0f,ccp(-width,0));
	CCMoveBy* moveb = CCMoveBy::create(60.0f,ccp(-width,0));
	CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(CMainCityBuild::relocation)),moveb,nullptr);
	m_fogLay->runAction(CCRepeatForever::create(sequence));

 	m_cloudPos = m_fogLay->getPosition();

// 	CCSprite *cloud1  =  (CCSprite *)(m_fogLay->findWidgetById("cloud1"));
 	m_cloudAPos = m_fogLay1->getPosition();

	CCMoveBy* move1 = CCMoveBy::create(120.0f,ccp(-width*2,0));
	CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(CMainCityBuild::relocation)));
	m_fogLay1->runAction(CCRepeatForever::create(sequence1));
}

void CMainCityBuild::relocation(CCNode* pSender)
{
	CLayout *cloud = (CLayout*)pSender;
	float width = 2616;/*cloud->getContentSize().width;*/
	CCPoint pos; 
	if (strcmp(cloud->getId(),"fog2")==0)
	{
		pos = ccpAdd(m_cloudPos,ccp(width,0));
		cloud->setPosition(pos);
	}
	else
	{
		cloud->setPosition(m_cloudAPos);
	}
}


void CMainCityBuild::runFogAction()
{
	//云运动
	CLayout *cloud  =  (CLayout *)(m_ui->findWidgetById("fog4"));
	float width = 2616;
	CCMoveBy* move = CCMoveBy::create(100.0f,ccp(width,0));
	CCMoveBy* moveb = CCMoveBy::create(100.0f,ccp(width,0));
	CCSequence *sequence = CCSequence::create(move,CCCallFuncN::create(this,callfuncN_selector(CMainCityBuild::relocationFog)),moveb,nullptr);
	cloud->runAction(CCRepeatForever::create(sequence));
	m_fogPos = cloud->getPosition();

	CLayout *cloud1  =  (CLayout *)(m_ui->findWidgetById("fog5"));
	m_fogAPos = cloud1->getPosition();

	CCMoveBy* move1 = CCMoveBy::create(200.0f,ccp(width*2,0));
	CCSequence *sequence1 = CCSequence::createWithTwoActions(move1,CCCallFuncN::create(this,callfuncN_selector(CMainCityBuild::relocationFog)));
	cloud1->runAction(CCRepeatForever::create(sequence1));
}

void CMainCityBuild::relocationFog(CCNode* pSender)
{
	CLayout *cloud = (CLayout*)pSender;
	float width = cloud->boundingBox().size.width;
	CCPoint pos; 
	if (strcmp(cloud->getId(),"fog4")==0)
	{
		pos = ccpAdd(m_fogPos,ccp(-width,0));
		cloud->setPosition(pos);
	}
	else
	{
		cloud->setPosition(m_fogAPos);
	}
}

void CMainCityBuild::cityHeroMovePoint()
{
	//武将移动的路径点
	CCNode *pointNode = nullptr;
	for (int i = 0; i < 4; i++)
	{
		pointNode = m_frontLay->getChildByTag(100+i);
		m_movePoint[i] = pointNode->getPosition();
	}

	//铁匠铺
	CLayout *tiePoint = (CLayout*)m_ui->findWidgetById("tiePoint");
	for (int i = 0; i < 3; i++)
	{
		m_tieQiangPos[i] = tiePoint->getChildByTag(i+1)->getPosition();	
	}
	m_garrsionPointMap[10] =m_tieQiangPos;

	CLayout *fieldPoint = (CLayout*)m_ui->findWidgetById("fieldPoint");
	for (int i = 0; i < 3; i++)
	{
		m_fieldPos[i] = fieldPoint->getChildByTag(i+1)->getPosition();
	}
	m_garrsionPointMap[3] =m_fieldPos;

	CLayout *goldPoint= (CLayout*)m_ui->findWidgetById("goldPoint");
	for (int i = 0; i < 3; i++)
	{
		m_goldPos[i] = goldPoint->getChildByTag(i+1)->getPosition();
	}
	m_garrsionPointMap[6] =m_goldPos;

	CLayout *comPoint= (CLayout*)m_ui->findWidgetById("composePoint");
	for (int i = 0; i < 3; i++)
	{
		m_composePos[i] = comPoint->getChildByTag(i+1)->getPosition();
	}
	m_garrsionPointMap[12] = m_composePos;
}

void CMainCityBuild::addMoveHero(CCity * cb, int i)
{
	//不需要人物出来走动
	CHero &hero = cb->heroList[0];

	if (m_frontLay->getChildByTag(200+cb->cityId))
	{
		return;
	}

	updateCityGarrsion(&hero,cb->cityId);
}


void CMainCityBuild::updateCityGarrsion(CHero *hero, int cityId)
{
	CCString *str = CCString::createWithFormat("BoneAnimation/%d.ExportJson",hero->thumb);
	CCNode *heroNode = CCNode::create();
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(str->getCString());
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(hero->thumb));
	if (outPutERRORMsg("StoryLayer::step AnimationData is NULL",animationData))return;
	CCArmature* at = CCArmature::create(ToString(hero->thumb));
	at->setTag(100);
	heroNode->addChild(at);
	heroNode->setAnchorPoint(CCPointCenter);
	at->getAnimation()->play("standby", 0.01f);
	heroNode->setScale(0.3f);

	heroNode->setTag(200+cityId);


	m_frontLay->addChild(heroNode, 995);


	CCPoint *pos;

// 	if (cityId==6)
// 	{
// 		heroNode->setPosition(m_fieldPos[0]);
// 		heroNode->setUserData(m_fieldPos);
// 		pos = m_fieldPos;
// 	}
// 	else if(cityId==3)
// 	{
// 		heroNode->setPosition(m_goldPos[0]);
// 		heroNode->setUserData(m_goldPos);
// 		pos = m_goldPos;
// 
// 	}
// 	else if (cityId==10)
// 	{
// 		heroNode->setPosition(m_tieQiangPos[0]);
// 		heroNode->setUserData(m_tieQiangPos);
// 		pos = m_tieQiangPos;
// 	}
	if (m_garrsionPointMap.find(cityId)!=m_garrsionPointMap.end())
	{
		pos = m_garrsionPointMap[cityId];
		heroNode->setPosition(pos[0]);
		heroNode->setUserData(pos);
	}
	else
	{
		pos = m_garrsionPointMap[12];
		heroNode->setPosition(pos[0]);
		heroNode->setUserData(pos);
	}

	CCPoint heroPos = heroNode->getPosition();

	int indexPos = random_range(0,2);
	//	CCPoint *pos = (CCPoint*)m_hero[i]->getUserData();
	m_movePos = pos[indexPos];/*m_movePoint[indexPos];*/

	if(m_movePos.x - heroNode->getPositionX()>0)
	{
		heroNode->getChildByTag(100)->setScaleX(1);
	}else
	{
		heroNode->getChildByTag(100)->setScaleX(-1);
	}
	at->getAnimation()->play("move", 0.01f);
	this->unschedule(schedule_selector(CMainCityBuild::moveHero));

	float distance = ccpDistance(m_movePos,heroNode->getPosition());

	CCMoveTo *move = CCMoveTo::create(distance*0.02,m_movePos);
	CCCallFuncN *call = CCCallFuncN::create(this, callfuncN_selector(CMainCityBuild::walkStop));
	heroNode->runAction(CCSequence::create(move,call,nullptr));
}

void CMainCityBuild::updateCityBuild(const TMessage& tMsg)
{
	if (tMsg.lParam)
	{
		CCity *city = (CCity*)tMsg.lParam;
// 		updateBuildTexture(city);
		delete tMsg.lParam;
	}
}

void CMainCityBuild::updateMobileShop(const TMessage& tMsg)
{
	UserData *data = DataCenter::sharedData()->getUser()->getUserData();
	if (data->getMobileShop()&&m_armature&&!m_armature->isVisible())
	{
		m_armature->setVisible(true);
		//m_armature->getAnimation()->play("tiao",-1,-1,0);
	}
	else if (m_armature&&!data->getMobileShop())
	{
		m_armature->setVisible(false);
	}
}

void CMainCityBuild::updateBuildTexture(CCity * city)
{
	if (city->cityId>0&&city->level>1)
	{
		int ret = 1;
		if (city->level>=3&&ret<6)
		{
			ret=2;
		}
		else if (city->level>=6&&city->level<=9)
		{
			ret=3;
		}
		else if (city->level>=10)
		{
			ret=4;
		}

		if (ret==1)
		{
			return;
		}
	
		CCString *buildStr = CCString::createWithFormat("mainCity/Build/%d_%d.png",city->cityId,ret);
		CButton *btn = (CButton*)(m_frontLay->getChildByTag(city->cityId));
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(buildStr->getCString());
		if (texture&&btn)
		{
			CCRect rect;
			CCSprite *normalSpr = (CCSprite*)btn->getNormalImage();
			rect.origin = normalSpr->boundingBox().origin;
			rect.size.width = texture->getContentSize().width;
			rect.size.height = texture->getContentSize().height;	
			normalSpr->setTexture(texture);	
			normalSpr->setTextureRect(rect);
			btn->setContentSize(rect.size);
// 			normalSpr->setAnchorPoint(ccp(0.5,0));
// 			normalSpr->setPositionY(normalSpr->getPositionY()-rect.size.height/2);
		}
	}
}

void CMainCityBuild::createBuildEffect()
{
	CLayout *effLay = (CLayout*)(m_ui->findWidgetById("strengEffect"));
	CCPoint firePos = ((CCNode*)effLay->getChildByTag(1))->getPosition();
	CCPoint lightPos1 = ((CCNode*)effLay->getChildByTag(2))->getPosition();
	CCPoint lightPos2 = ((CCNode*)effLay->getChildByTag(3))->getPosition();

	if (m_cityMap[4].isOpen)
	{	
		addStrengthenAnim(firePos, effLay);
	}

	if (m_cityMap[15].isOpen)
	{
		 addComposeAnim(lightPos1, lightPos2, effLay);
	}
	
	if (m_cityMap[6].isOpen)
	{	
		//培植动画
		addCulAnim(effLay);
	}

	

	if (m_cityMap[13].isOpen)
	{
		CCPoint recruitPos = ((CCNode*)effLay->getChildByTag(5))->getPosition();
		CCAnimation *recruitAnim = AnimationManager::sharedAction()->getAnimation("7001");
		recruitAnim->setDelayPerUnit(0.2f);
		CCSprite *recruitSpr = createAnimationSprite("skill/7001.png",ccpAdd(recruitPos,ccp(-575, 164)),recruitAnim,true);
		m_frontLay->addChild(recruitSpr);

		{
			CCPoint recruitPos = ((CCNode*)effLay->getChildByTag(5))->getPosition();
			CCAnimation *recruitAnim = AnimationManager::sharedAction()->getAnimation("7001");
			recruitAnim->setDelayPerUnit(0.2f);
			CCSprite *recruitSpr = createAnimationSprite("skill/7001.png",ccpAdd(recruitPos,ccp(-850, 194)),recruitAnim,true);
			m_frontLay->addChild(recruitSpr);
		}

		CCAnimation *fireAnim = AnimationManager::sharedAction()->getAnimation("8006");
		fireAnim->setDelayPerUnit(0.1f);
		CCPoint pos = ccp(140, 370);
		CImageView *fire = CImageView::create("skill/8006.png");
		fire->setTouchEnabled(true);
		fire->runAction(CCRepeatForever::create(CCAnimate::create(fireAnim)));
		fire->setPosition(pos);
		m_frontLay->addChild(fire,-1);
	}

	if (m_cityMap[16].isOpen)
	{
		CCNode *stone1 = m_frontLay->findWidgetById("stone1");
		stone1->setVisible(true);
		stone1->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.5f,ccp(0,20)),CCMoveBy::create(0.5f,ccp(0,-20)))));
		CCNode *stone2 = m_frontLay->findWidgetById("stone2");
		stone2->setVisible(true);
		stone2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.5f,ccp(0,20)),CCMoveBy::create(0.5f,ccp(0,-20)))));
		CCNode *stone3 = m_frontLay->findWidgetById("stone3");
		stone3->setVisible(true);
		stone3->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.5f,ccp(0,20)),CCMoveBy::create(0.5f,ccp(0,-20)))));
	}

	CCPoint goldPos = ((CCNode*)effLay->getChildByTag(7))->getPosition();
	CCAnimation *goldAnim = AnimationManager::sharedAction()->getAnimation("7005");
	goldAnim->setDelayPerUnit(0.3f);
	CCSprite *goldSpr = createAnimationSprite("skill/7005.png",goldPos,goldAnim,true);
	m_frontLay->addChild(goldSpr);

// 	CLayout *composPosLay = (CLayout*)(m_ui->findWidgetById("composePoint"));
// 	CCPoint composPos1 = composPosLay->getChildByTag(1)->getPosition();
// 	CCAnimation *composAnim1 = AnimationManager::sharedAction()->getAnimation("7009");
// 	composAnim1->setDelayPerUnit(0.2f);
// 	CCSprite *composSpr1 = createAnimationSprite("skill/7009.png",composPos1,composAnim1,true);
// 	m_frontLay->addChild(composSpr1);

	CCAnimation *lightAnim = AnimationManager::sharedAction()->getAnimation("7002");
	lightAnim->setDelayPerUnit(0.2f);
	CCSprite *light = createAnimationSprite("skill/7002.png",ccpAdd(goldPos,ccp(5,165)),lightAnim,true);
	m_frontLay->addChild(light);

	CCSprite *gear1 = (CCSprite*)(m_frontLay->findWidgetById("gear1"));
	CCSprite *gear2 = (CCSprite*)(m_frontLay->findWidgetById("gear2"));
	CCSprite *gear3 = (CCSprite*)(m_frontLay->findWidgetById("gear3"));
	CCSprite *gear4 = (CCSprite*)(m_frontLay->findWidgetById("gear4"));
	CCSprite *gear5 = (CCSprite*)(m_frontLay->findWidgetById("gear5"));
	CCSprite *gear6 = (CCSprite*)(m_frontLay->findWidgetById("gear6"));
	gear1->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,360.0f)));
	gear2->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,360.0f)));
	gear3->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,360.0f)));
	gear4->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,-360.0f)));
	gear5->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,-360.0f)));
	gear6->runAction(CCRepeatForever::create(CCRotateBy::create(15.0f,360.0f)));

	CCAnimation *bloodAnim = AnimationManager::sharedAction()->getAnimation("7036");
	bloodAnim->setDelayPerUnit(0.3f);
	CCSprite *blood = (CCSprite*)(m_frontLay->findWidgetById("blood"));
	blood->runAction(CCRepeatForever::create(CCAnimate::create(bloodAnim)));
}

void CMainCityBuild::onTimeWaitCityAction(CCNode* pSender){
	CButton *btn = (CButton*)pSender;
	if (btn != NULL)
	{
		btn->executeClickHandler(btn);
	}
}

void CMainCityBuild::moveFrontByPos(CCPoint pos)
{
// 	m_frontLay->setPositionX(m_frontLay->getPositionX()+pos.x);
// 	m_bgLay->setPositionX(m_bgLay->getPositionX()+pos.x*0.7);
	//if (m_frontLay->getPositionX()+pos.x<m_minX)
	{
		m_frontLay->setPositionX(m_minX);
		m_bgLay->setPositionX(792);
	}
}

void CMainCityBuild::updateFuctionOpen(const TMessage& tMsg)
{
	int tag = tMsg.nMsg;
	if (tag<20&&tag>0)
	{
		CLayout *zitiLay = dynamic_cast<CLayout*>(m_frontLay->findWidgetById("ziti"));
		CCNode *font = zitiLay->getChildByTag(tag*100+1);
		font->setVisible(true);
		CCNode *di = zitiLay->getChildByTag(tag*100+2);
		di->setVisible(true);

		CLayout *effLay = (CLayout*)(m_ui->findWidgetById("strengEffect"));
		CCPoint firePos = ((CCNode*)effLay->getChildByTag(1))->getPosition();
		CCPoint lightPos1 = ((CCNode*)effLay->getChildByTag(2))->getPosition(); 
		CCPoint lightPos2 = ((CCNode*)effLay->getChildByTag(3))->getPosition();

		CButton *btn = dynamic_cast<CButton*>(m_frontLay->getChildByTag(tag));
		if (btn)
		{
			btn->setEnabled(true);
		}

		if (tag==4)
		{
			addStrengthenAnim(firePos, effLay);
		}
		else if (tag==12)
		{
			addComposeAnim(lightPos1, lightPos2, effLay);
		}
		else if (tag==6)
		{
			addCulAnim(effLay);
		}
	}
}

void CMainCityBuild::addCulAnim(CLayout * effLay)
{
	CCPoint culPos = ((CCNode*)effLay->getChildByTag(4))->getPosition();
	CCAnimation *culAnim = AnimationManager::sharedAction()->getAnimation("7006");
	culAnim->setDelayPerUnit(0.2f);
	CCSprite *culSpr = createAnimationSprite("skill/7006.png",culPos,culAnim,true);
	m_frontLay->addChild(culSpr);

	CCAnimation *composAnim2 = AnimationManager::sharedAction()->getAnimation("7010");
	composAnim2->setDelayPerUnit(0.15f);
	CCSprite *composSpr2 = createAnimationSprite("skill/7010.png",ccpAdd(culPos,ccp(-45,90)),composAnim2,true);
	m_frontLay->addChild(composSpr2);	
}

void CMainCityBuild::addStrengthenAnim(CCPoint firePos, CLayout * effLay)
{
	CCAnimation *fireAnim = AnimationManager::sharedAction()->getAnimation("7007");
	CCSprite *fire = createAnimationSprite("skill/7007.png",firePos,fireAnim,true);
	m_frontLay->addChild(fire);

	CCPoint recruitPos1 = ((CCNode*)effLay->getChildByTag(8))->getPosition();
	CCAnimation *recruitAnim1 = AnimationManager::sharedAction()->getAnimation("7004");
	recruitAnim1->setDelayPerUnit(0.2f);
	CCSprite *recruitSpr1 = createAnimationSprite("skill/7004.png",ccpAdd(recruitPos1,ccp(0,15)),recruitAnim1,true);
	m_frontLay->addChild(recruitSpr1);
}

void CMainCityBuild::addComposeAnim(CCPoint lightPos1, CCPoint lightPos2, CLayout * effLay)
{
	CCAnimation *lightAnim1 = AnimationManager::sharedAction()->getAnimation("7008");
	lightAnim1->setDelayPerUnit(0.3f);
	CCSprite *light1 = createAnimationSprite("skill/7008.png",lightPos1,lightAnim1,true);
	m_frontLay->addChild(light1);

	CCAnimation *lightAnim2 = AnimationManager::sharedAction()->getAnimation("7008");
	lightAnim2->setDelayPerUnit(0.3f);
	CCSprite *light2 = createAnimationSprite("skill/7008.png",lightPos2,lightAnim2,true);
	m_frontLay->addChild(light2);	

// 	CCPoint composPos = ((CCNode*)effLay->getChildByTag(6))->getPosition();
// 	CCAnimation *composAnim = AnimationManager::sharedAction()->getAnimation("7003");
// 	composAnim->setDelayPerUnit(0.2f);
// 	CCSprite *composSpr = createAnimationSprite("skill/7003.png",composPos,composAnim,true);
// 	m_frontLay->addChild(composSpr);
}

void CMainCityBuild::show(CCObject* pObj)
{
	m_iIndexForShow++;
	if(m_iIndexForShow>0)
	{
		m_ui->setVisible(true);
		m_cityUi->setVisible(true);		
	}
	CCLOG("CMainCityBuild::show_%d", m_iIndexForShow);
}

void CMainCityBuild::hide(CCObject* pObj)
{
	m_iIndexForShow--;
	if(m_iIndexForShow<=0)
	{
		m_ui->setVisible(false);
		m_cityUi->setVisible(false);
	}
	CCLOG("CMainCityBuild::hide_%d", m_iIndexForShow);
}

void CMainCityBuild::updateGameTip(const TMessage& tMsg)
{
	CGameTips *ct = CTaskControl::getInstance()->getGameTips();
	showNoticeTip(ct);
}

void CMainCityBuild::showNoticeTip(CGameTips * ct)
{
	CCNode *ziti = (CCNode*)(m_frontLay->findWidgetById("ziti"));

	if (ct->composeTips)
	{
		CCNode *circle = ziti->getChildByTag(1203);
		circle->setVisible(true);
	}
	if (ct->strenTips||ct->risingTips)
	{
		CCNode *circle = ziti->getChildByTag(403);
		circle->setVisible(true);
	}
	if (ct->lotteryTips)
	{
		CCNode *circle = ziti->getChildByTag(1003);
		circle->setVisible(true);
	}
	if (ct->farmTips)
	{
		CCNode *circle = ziti->getChildByTag(603);
		circle->setVisible(true);
	}
	if (ct->bountyTips)
	{
		CCNode *circle = ziti->getChildByTag(1503);
		circle->setVisible(true);
	}
}

void CMainCityBuild::hideTips(int tag)
{
	CCNode *ziti = (CCNode*)(m_frontLay->findWidgetById("ziti"));
	CCNode *circle = ziti->getChildByTag(100*tag+3);
	circle->setVisible(false);
}

void CMainCityBuild::recvWorldBossNotice( int type, google::protobuf::Message *msg )
{
	CCNode *ziti = (CCNode*)(m_frontLay->findWidgetById("ziti"));
	CCSprite *red = (CCSprite*)ziti->getChildByTag(1603);
	red->setVisible(true);
}

void CMainCityBuild::showPvp()
{
	//TODO
	//隐藏UI窗口
	//滑动屏幕到悬浮塔为水平中心
	//重置锚点到塔上面( 0.5f, 0.6f)
	//塔慢慢移动到屏幕中间偏上方并进行放大
	//生成PVP场景，用层，不用场景，可能有些东西还需要显示
	//PVP场景进场动画

	m_bTouchLock = true;

	unscheduleUpdate();

	float fFgPosX = m_maxX - 100;
	float fBgPosX = m_iBgLayerMaxLeftBasePos - 70;

	float fTime = fabs(m_frontLay->getPositionX()-fFgPosX)/700;

	m_frontLay->runAction(
		CCEaseSineInOut::create(CCMoveTo::create(fTime, ccp(fFgPosX, m_frontLay->getPositionY())))
		);
	m_bgLay->runAction(
		CCEaseSineInOut::create(CCMoveTo::create(fTime, ccp(fBgPosX, m_fogLay->getPositionY())))
		);
	
	m_cityUi->setVisible(false);
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);

	//先初始化资源，避免动画过程中卡顿
	CMainScene *pScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if( pScene )
	{
		CPvpGateLayer *pLayer = pScene->addPvpGateLayer();
		int iLevel = CMainCityControl::getInstance()->getCityDataById(Btn_Tower).level;
		pLayer->setGateLevel(iLevel);
	}

	m_ui->runAction(CCSequence::create(
		CCDelayTime::create(fTime),
		CCCallFunc::create(this, callfunc_selector(CMainCityBuild::showPvpCallbackForResetAnchorPoint)),
		CCSpawn::create(
			CCShake::create(0.3f, 8),
			CCMoveTo::create(0.3f, ccp(VCENTER.x, VCENTER.y)),
			CCScaleTo::create(0.3f, 2.3f),
			CCSequence::createWithTwoActions(CCDelayTime::create(0.15f), CCCallFunc::create(this, callfunc_selector(CMainCityBuild::showPvpCallBack))),
			nullptr
		),
		nullptr));
}


void CMainCityBuild::resetPosToTower( CCObject *pSender )
{
	float fFgPosX = m_maxX - 100;
	float fBgPosX = m_iBgLayerMaxLeftBasePos - 70;

	m_frontLay->setPositionX(fFgPosX);
	m_bgLay->setPositionX(fBgPosX);
}


void CMainCityBuild::showPvpCallbackForResetAnchorPoint()
{
	ResetAnchorPointAndKeepSamePos(m_ui, ccp(0.5f, 0.7f));
}


void CMainCityBuild::showPvpCallBack()
{
	//TODO
	CMainScene *pScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	CPvpGateLayer *pPvpGateLayer = pScene->getPvpGateLayer();
	//播放动画
	pPvpGateLayer->showEffectIn();
}

void CMainCityBuild::backFromPvp( CCObject *pSender )
{
	m_bTouchLock = true;

	unscheduleUpdate();

	resetPosToTower(nullptr);
	m_cityUi->setVisible(false);

	//先重置位置，上去时设置过的
	m_ui->setScale(1.0f);
	backFromPvpForResetAnchorPoint();

	m_ui->setPosition( VCENTER );
	ResetAnchorPointAndKeepSamePos(m_ui, ccp(0.5f, 0.7f));
	m_ui->setScale( 2.3f );

	m_ui->runAction(CCSequence::create(
		CCDelayTime::create(0.4f),
		CCScaleTo::create(1.5f, 1.0f),
		CCCallFunc::create(this, callfunc_selector(CMainCityBuild::backFromPvpForResetAnchorPoint)),
		CCCallFunc::create(this, callfunc_selector(CMainCityBuild::backFormPvpCallBack)),
		nullptr));
}

void CMainCityBuild::backFormPvpCallBack()
{
	m_cityUi->setVisible(true);
	NOTIFICATION->postNotification(SHOW_TOP_LAYER);

	m_bTouchLock = false;

	scheduleUpdate();

	//移除无用的资源
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

void CMainCityBuild::backFromPvpForResetAnchorPoint()
{
	ResetAnchorPointAndKeepSamePos(m_ui, ccp(0.5f, 0.5f));
}

void CMainCityBuild::autoToPvp()
{
	//TODO
	//隐藏UI窗口
	//滑动屏幕到悬浮塔为水平中心
	//重置锚点到塔上面( 0.5f, 0.6f)
	//塔慢慢移动到屏幕中间偏上方并进行放大
	//生成PVP场景，用层，不用场景，可能有些东西还需要显示
	//PVP场景进场动画

	m_bTouchLock = true;

	unscheduleUpdate();

	m_cityUi->setVisible(false);
	NOTIFICATION->postNotification(HIDE_TOP_LAYER);

	//先初始化资源，避免动画过程中卡顿
	CMainScene *pScene = dynamic_cast<CMainScene*>(CSceneManager::sharedSceneManager()->getCurrScene());
	if( pScene )
	{
		CPvpGateLayer *pLayer = pScene->addPvpGateLayer();
		int iLevel = CMainCityControl::getInstance()->getCityDataById(Btn_Tower).level;
		pLayer->setGateLevel(iLevel);
		pLayer->showAsynPvp();
	}

	showPvpCallBack();
}