 /************************************************************* 
  *
  *
  *		Data : 2016.6.22
  *	
  *		Name : 
  *
  *		Author : Lin_Xiancheng
  *
  *		Description : 
  *
  *
  *************************************************************/

#include "Battle/BattleScene/BattleScene.h"
#include "tools/commonDef.h"
#include "Battle/BattleCenter.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "Battle/DropItem.h"
#include "Battle/Landform/TrapLayer.h"
#include "Battle/WarControl.h"
#include "scene/layer/BackLayer.h"
#include "Battle/ComBatLogic.h"
#include "Battle/BattleLayer/StoryLayer.h"
#include "Battle/BattleLayer/BattleMapLayer.h"
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/CombatGuideData.h"
#include "Battle/CombatGuideLayer.h"
#include "Battle/CombatGuideManage.h"
#include "common/CommonFunction.h"
#include "Battle/EffectData.h"
#include "tools/CCShake.h"
#include "Battle/BattleMessage.h"
#include "Battle/RoleMacro.h"
#include "Battle/WarBackLayer.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "netcontrol/CPlayerControl.h"
#include "Battle/WarFailLayer.h"
#include "Battle/WarWinLayer.h"
#include "Battle/WorldBossEndLayer.h"
#include "Battle/BattleClose.h"
#include "Battle/BattleDataCenter.h"
#include "Battle/BattleModel.h"
#include "Battle/PvpEndLayer.h"
#include "model/DataCenter.h"
#include "Battle/SpineFloor/SpineFloorLayer.h"

namespace BattleSpace
{
	BattleScene::BattleScene()
	:m_MapLayer(nullptr),m_AliveLayer(nullptr),m_StoryLayer(nullptr),mSpineEffectLayer(nullptr)
	,m_MoveLayer(nullptr),m_UILayer(nullptr),_dropItem(nullptr),mMoveState(true)
	,m_Loginc(nullptr),m_Touch(nullptr),mBackLayer(nullptr),mBattleClose(nullptr)
	{}

	BattleScene::~BattleScene()
	{
		CC_SAFE_RELEASE(_dropItem);
		_dropItem = nullptr;
	}

	void BattleScene::onEnter()
	{
		CScene::onEnter();
		DataCenter::sharedData()->getUser()->setoldLevel();									//设置开始战斗前等级
		LayerMoveEnd(CCInteger::create((int)StoryType::eBeginStory));						//这个方法不应该放在这里的发条消息也能解决的问题
	}

	void BattleScene::onEnterTransitionDidFinish()
	{
		m_GuideLayer->initScene(this);
	}

	void BattleScene::onExit()
	{
		CScene::onExit();
		this->unscheduleUpdate();
		RemoveEvent();
	}

	void BattleScene::onCreate()
	{
		m_MoveLayer = CCNode::create();					//可以移动的节点包含地图和武将
		addChild(m_MoveLayer);
		//if (BattleModelManage->isPvEBattle())
		//	m_MoveLayer->setScale(0.85f);

		m_MapLayer = BattleMapLayer::create();
		m_MoveLayer->addChild(m_MapLayer);

		mTrapLayer = TrapLayer::create();
		m_MoveLayer->addChild(mTrapLayer);

		mSpineEffectLayer = SpineFloorLayer::create();
		m_MoveLayer->addChild(mSpineEffectLayer);

		m_AliveLayer = BattleRoleLayer::create();
		m_MoveLayer->addChild(m_AliveLayer);

		m_UILayer = WarControl::create();
		this->addChild(m_UILayer);

		//物品掉落功能取消了
		_dropItem = DropItem::create();
		_dropItem->retain();
		_dropItem->setWarUi(m_UILayer,m_AliveLayer);

		BackLayer* bkLayer = BackLayer::create();
		this->addChild(bkLayer);

		m_GuideLayer = CombatGuideLayer::create();
		m_GuideLayer->setTouchEnabled(false);
		this->addChild(m_GuideLayer);
		ManageCenter->getCombatGuideMg()->setGuideLayer(m_GuideLayer);

		m_StoryLayer = StoryLayer::create();
		m_StoryLayer->setVisible(false);
		m_StoryLayer->setTouchEnabled(false);
		addChild(m_StoryLayer, 99);

		m_Loginc = CombatLogic::create();
		addChild(m_Loginc);									//添加到父类才能开启定时器

		mBackLayer = WarBackLayer::create();
		mBackLayer->setVisible(false);
		addChild(mBackLayer);

		mBattleClose = BattleClose::create();
		addChild(mBattleClose);
	}
	//添加事件处理监听等
	void BattleScene::AddEvent()
	{
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,AliveLayerPriority,true);
		if( m_UILayer ) m_UILayer->AddEvent();
		if( m_AliveLayer ) m_AliveLayer->addEvent();
		if( m_MapLayer ) m_MapLayer->addEvent();
		bNotification->addObserver(this,callfuncO_selector(BattleScene::cReturnLayer),MsgReturnLayer,nullptr);
		CNetClient::getShareInstance()->registerMsgHandler(ExitStage,this,CMsgHandler_selector(BattleScene::OnBattleFinish));
		CNetClient::getShareInstance()->registerMsgHandler(BossFinishReqMsg,this,CMsgHandler_selector(BattleScene::onWordBossFinish));
		CNetClient::getShareInstance()->registerMsgHandler(PvpOverReqMsg, this, CMsgHandler_selector(BattleScene::onPVEBattleFinish));
	}
	//移除事件
	void BattleScene::RemoveEvent()
	{
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
		CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
		if( m_UILayer ) m_UILayer->RemoveEvent();
		if( m_AliveLayer) m_AliveLayer->removeEvent();
		if( m_MapLayer ) m_MapLayer->removeEvent();
		bNotification->removeAllObservers(this);
		CNetClient::getShareInstance()->unRegisterAllMsgHandler(this);
	}

	bool BattleScene::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (! m_Touch)
		{
			m_Touch = pTouch;
			m_StartPos = pTouch->getLocation();
		}
		return true;
	}
	//拖动地图
	void BattleScene::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
	{
#if !BATTLE_TEST
		if (!BattleManage->getNormal() || 
			!BattleModelManage->layerMove())
			return;
#endif
		if (m_Touch != pTouch || !mMoveState)
			return ;
		CCPoint pMove = m_Touch->getLocation();
		float dx = pMove.x - m_StartPos.x;							//地图只能x轴移动
		float newX = dx + m_MoveLayer->getPositionX();
		if( newX < BattleCoords->CoordsMin() )
			newX = BattleCoords->CoordsMin();
#if BATTLE_TEST
		if( newX > BattleCoords->CoordsMax() )
			newX = BattleCoords->CoordsMax();
#else
		if (newX > 0)newX = 0;
#endif
		m_StartPos = pMove;											//不断更新触摸初始值
		if (ManageCenter->getCombatGuideMg()->IsGuide())//引导状态进行锁屏处理
		{
			CombatGuideStep* step = ManageCenter->getCombatGuideMg()->getCurrStep();
			if (step->getLockLayer())
				return ;
		}
		float px = (newX - m_MoveLayer->getPositionX()) * 0.3f;	//已经偏移了量太多了偏移少一些回来
		float layx = 0;
		CLayout *lay = dynamic_cast<CLayout*>(m_MapLayer->getMaplayer()->findWidgetById("yuanjing"));
		layx = lay->getPositionX();	
		m_MoveLayer->setPositionX(newX);	
		lay->setPositionX(layx-px);					
	}

	void BattleScene::ccTouchCancelled( CCTouch *pTouch, CCEvent *pEvent )
	{
		if (pTouch == m_Touch)
			m_Touch = nullptr;
	}

	void BattleScene::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (pTouch == m_Touch)
			m_Touch = nullptr;
	}		

	void BattleScene::LayerMoveEnd(CCObject* ob)
	{
#if BATTLE_TEST
		bNotification->postNotification(MsgCreateStory,ob);
#else
		if (BattleManage->getStageIndex())
		{
			bNotification->postNotification(MsgCreateStory,ob);
		}else{
			if (((CCInteger*)ob)->getValue() == (int)StoryType::eBeginStory)
				bNotification->postNotification(B_PlayBeginAnimation);
			else
				bNotification->postNotification(MsgCreateStory,ob);
		}
#endif
	}

	void BattleScene::cReturnLayer( CCObject* ob )
	{
		if (mBackLayer->isVisible())
		{
			mBackLayer->hide();
			m_Loginc->onResume();
		}else{
			mBackLayer->show();
			m_Loginc->onPause();
		}
	}

	void BattleScene::OnBattleFinish( int pType, google::protobuf::Message *msg )
	{
		if(pType != ExitStage)
			return;
		mBattleClose->setRecvFinish(true);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		if (res->win())
		{
			WarWinLayer *layer = WarWinLayer::create();
			this->addChild(layer);
			layer->processBattleFinish(pType, msg);
		}else{
			WarFailLayer *layer = WarFailLayer::create();
			this->addChild(layer);
		}
	}

	void BattleScene::onWordBossFinish( int pType, google::protobuf::Message *msg )
	{
		if(pType != BossFinishReqMsg)
			return;
		mBattleClose->setRecvFinish(true);
		BattleFinishRep *res = (BattleFinishRep*)msg;
		WorldBossEndLayer * layer = WorldBossEndLayer::create();
		this->addChild(layer);
		layer->processBattleFinish(pType, msg);
	}

	void BattleScene::onPVEBattleFinish( int type, google::protobuf::Message *msg )
	{
		if(type != PvpOverReqMsg)
			return;
		mBattleClose->setRecvFinish(true);
		PvpEndLayer *pLayer = PvpEndLayer::create();
		pLayer->setBattleResult(mBattleClose->getFinishData().res);
		this->addChild(pLayer);
		pLayer->processBattleFinish(type, msg);
	}

};