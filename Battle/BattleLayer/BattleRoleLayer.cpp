
#include "Battle/BattleLayer/BattleRoleLayer.h"
#include "Battle/BattleCenter.h"
#include "Battle/AnimationManager.h"
#include "common/CommonFunction.h"
#include "Battle/ConstNum.h"
#include "Battle/SkillRange.h"
#include "tools/commonDef.h"
#include "common/color.h"
#include "Battle/RoleObject/RoleObject.h"
#include "Battle/MoveRule.h"
#include "Battle/EffectObject.h"
#include "tools/CCShake.h"
#include "Battle/WarManager.h"
#include "Battle/CoordsManage.h"
#include "Battle/CombatGuideManage.h"
#include "Battle/CombatGuideData.h"
#include "Battle/BattleScene/BattleScene.h"
#include "tools/ShowTexttip.h"
#include "Battle/ComBatLogic.h"
#include "Battle/SkillRange.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "Battle/RoleConfig.h"
#include "Battle/BattleMessage.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
namespace BattleSpace
{
	BattleRoleLayer::BattleRoleLayer()
		:mTouchRole(nullptr),m_grid(0),mRoleNode(0),mtestState(true)
		,mMoveRole(nullptr),m_TouchAliveBtn(false),mManage(nullptr)
		,m_LayerColor(nullptr),mMovetParent(nullptr),m_TouchOffs(0,0)
		,mDefunt(nullptr)
	{}
	BattleRoleLayer::~BattleRoleLayer()
	{
		CC_SAFE_RELEASE(mDefunt);				//在管理器中创建一个默认的
		mDefunt = nullptr;
		mTouchRole = nullptr;
	}

	void BattleRoleLayer::onEnter()
	{
		BaseLayer::onEnter();
		this->scheduleUpdate();
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::LayerShake),B_Shark,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::roleEntranceBattle),B_EntranceBattle,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::changeTestState),B_ChangeMoveState,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::initActobject),MsgCreateRoleObject,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::roleStand),MsgRoleStand,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::removeMessage),MsgReleaseTouch,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::createBatchMonster),MsgNextBatchEnemy,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::heroWinAction),MsgPlayWinEffect,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(BattleRoleLayer::changeLight),MsgChangeLayerLight,nullptr);
	}

	void BattleRoleLayer::onExit()
	{
		this->unscheduleUpdate();
		removeEvent();
		NOTIFICATION->removeAllObservers(this);
		BaseLayer::onExit();
	}

	bool BattleRoleLayer::init()
	{
		if (!BaseLayer::init())
			return false;
		this->setVisible(true);
		this->setAnchorPoint(ccp(0,0));
		this->setTouchPriority(AliveLayerPriority);
		this->setTouchEnabled(false);
		this->setIsShowBlack(false);
		mMovetParent = CCNode::create();
		addChild(mMovetParent);
		mRoleNode = CCNode::create();
		addChild(mRoleNode);
		mManage = BattleManage;
		createMoveTarget();
		createLayerColor();
		createActObjects();
		return true;
	}

	void BattleRoleLayer::createMoveTarget()
	{
		//修改创建方法后,需要创建一个默认的对象再管理器中，用于创建默认的武将
		mDefunt = BaseRole::create();
		mDefunt->retain();
		mMoveRole = RoleObject::create();
		mMoveRole->setBaseRole(mDefunt);
		mMoveRole->setVisible(false);
		addChild(mMoveRole); 
	}

	void BattleRoleLayer::createLayerColor()
	{
		m_LayerColor = CCLayerColor::create(ccc4(0,0,0,200));
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		m_LayerColor->setContentSize(size*4);
		m_LayerColor->setPosition(ccp(-size.width, -size.height));
		m_LayerColor->setVisible(false);
		mRoleNode->addChild(m_LayerColor);
	}

	void BattleRoleLayer::createActObjects()
	{
		vector<BaseRole*>tMonsters;
		mManage->initMonsters(tMonsters,true);
		for (auto tMonster : tMonsters)
			initActobject(tMonster);
		vector<BaseRole*>tOthers;
		mManage->initOtherCamp(tOthers,true);
		for (auto tRole:tOthers)
			initActobject(tRole);
		initActobject(mManage->getAliveByGrid(C_CAPTAINSTAND));
	}

	void BattleRoleLayer::AddActToGrid(RoleObject* pRoleObject,int pGrid)
	{
		BaseRole* tRole = pRoleObject->getBaseRole();
		if (tRole->getDelaytime()>0)pGrid = 0;
		if (pGrid == INVALID_GRID&&tRole->getEnemy())
		{
			pGrid = CCRANDOM_0_1()*3+1;
			tRole->setGridIndex(pGrid);
		}
		pRoleObject->countOffs(BattleCoords->getPoint(pGrid));
		if (tRole->getOtherCamp() && !tRole->getEnemy())
			pRoleObject->setMoveState(sStateCode::eWalkState);
		if(pRoleObject->getParent() == nullptr)
		{
			mRoleNode->addChild(pRoleObject);
			pRoleObject->initMoveObject(mMovetParent);
			NOTIFICATION->postNotification(MsgRoleGridChange,tRole);				//武将首次进入战场做位置刷新处理
		}
	}
	//绘制战场武将
	void BattleRoleLayer::initActobject(CCObject* ob)
	{
		if ( !ob )return;
		BaseRole* pRole = (BaseRole*)ob;
		if (pRole->getRoleObject())
		{
			CCLOG("[ *TIPS ] BattleRoleLayer::createAlive alive Actobject Repeat");
			return;
		}
		RoleObject* tRoleObject = RoleObject::create();							//创建显示的对象
		tRoleObject->setBaseRole(pRole);											//显示对象与逻辑对象绑定
		pRole->setRoleLayer(this);
		if (pRole->getEnemy() || pRole->getFatherID())
		{
			//EffectObject* eff = EffectObject::create("304");
			//eff->setEffAnchorPoint(0.5f,0.5f);
			//eff->setPosition(ccp(0,GRID_HEIGHT*1.0f));
			//tRoleObject->addChild(eff);
			//tRoleObject->setDropItem(tRoleObject->getAlive()->getBaseData()->getRoleDrop());
			//PlayEffectSound(SFX_517);
		}
		AddActToGrid(tRoleObject,pRole->getGridIndex());
	}
	void BattleRoleLayer::addEvent(){ this->setTouchEnabled(true); }
	void BattleRoleLayer::removeEvent(){ this->setTouchEnabled(false); }

	void BattleRoleLayer::removeMessage(CCObject* ob)
	{
		ccTouchCancelled(nullptr,nullptr);
		removeEvent();
		addEvent();
	}
	//不断刷新武将的Z轴值
	void BattleRoleLayer::update(float delta)
	{
		CCArray* arr = mRoleNode->getChildren();
		CCObject* object;
		CCARRAY_FOREACH(arr,object)
		{
			RoleObject* act = dynamic_cast<RoleObject*>(object);
			if(!act || object == mMoveRole) continue;
			int zorder = -act->getPositionY();
			if(act->getParent()) 
				act->getParent()->reorderChild(act,zorder);	//z排序
			act->updatePosition(delta);
		}
		if( mMoveRole->getParent() ) 
			mMoveRole->getParent()->reorderChild(mMoveRole,200);			//刷新移动武将Z值排序
	}
	//初始化触摸武将存储的信息。
	void BattleRoleLayer::roleEntranceBattle(CCObject* ob)
	{
		BaseRole* tRole = (BaseRole*)ob;
		initActobject(tRole);
		initTouchAlive(tRole);
		m_TouchAliveBtn = true;
	}

	void BattleRoleLayer::initMoveActObject( RoleObject* pRoleObject )
	{
		mDefunt->setConfigData(mTouchRole->getConfigData());
		if (mMoveRole->getModel() != pRoleObject->getModel())
		{
			mMoveRole->setModel(pRoleObject->getModel());	//移动的目标是模型是被点击的目标模型'
			mMoveRole->setoffs(pRoleObject->getoffs());		//武将原来相对于格子的偏移量
			mMoveRole->getBody()->setScale(pRoleObject->getBody()->getScale());
		}
		if (pRoleObject->getBaseRole()->getGridIndex())
		{
			if(pRoleObject->getMoveObject())
				mMoveRole->setPosition(pRoleObject->getMoveObject()->getPosition());
		}else{
			mMoveRole->setPosition(ccp(-500,100));
		}
		mMoveRole->setVisible(true);
	}

	void BattleRoleLayer::lucencyActObject(bool lucency)
	{
		CCArray*ob_Arr = getAlivesOb(AliveType_Hero);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(ob_Arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			BaseRole* tRole = act->getBaseRole();	
			if (tRole->getCaptain())continue;
			setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),255);
			if (lucency)
				setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),100);
		}
	}

	void BattleRoleLayer::initTouchAlive(BaseRole* pRole)
	{
		mTouchRole = pRole;
		pRole->setInTouchState(true);
		pRole->setTouchGrid(pRole->getGridIndex());
		pRole->getRoleObject()->showThis();
		NOTIFICATION->postNotification(B_RoleAttackCostArea, pRole);
		initMoveActObject(pRole->getRoleObject());
		if (!pRole->getBattle())								//上阵武将才做透明处理
			return;
		lucencyActObject(true);
	}

	bool BattleRoleLayer::touchInAlive( int pGrid, const CCPoint& p )
	{
		vector<BaseRole*> tHeros = mManage->inBattleHeros();
		for (auto tHero : tHeros)
		{
			if (tHero->getCallType()!=sCallType::eCommon || tHero->getCriAtk() || !tHero->getMove())
				continue;
			MoveObject* tMove = tHero->getMoveObject();
			if (!tMove)continue;
			for (auto tGrid :tMove->grids)
			{
				if (tGrid!=pGrid)																//当触摸在mo的grid上就没有问题了,将当前触摸的点求出如果触摸在grid上的偏移量来
					continue;
				CCPoint upoint = tMove->getParent()->convertToWorldSpace(tMove->getPosition());	//转化为世界坐标进行偏移量处理
				m_TouchOffs = upoint - p;													//加减号被重载了,出触摸偏移量
				initTouchAlive(tHero);
				return true;
			}
		}
		return false;
	}

	bool BattleRoleLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (m_TouchAliveBtn)
		{
			m_TouchAliveBtn = false;
			return true;
		}
		CCPoint p = this->convertToNodeSpace(pTouch->getLocation());
		int grid = BattleCoords->getGridIndex(p);
		return touchInAlive(grid,  pTouch->getLocation());
	}

	int BattleRoleLayer::getTouchGrid( CCTouch* pTouch )
	{
		CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs;
		return BattleCoords->getGridIndex(p-mMoveRole->getoffs());								//移动目标所站的实际格子要减去偏移量
	}

	void BattleRoleLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (!mMoveRole->isVisible())
			return;
		CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs;
		int grid = getTouchGrid(pTouch);
		mMoveRole->setPosition(p);
		if (grid != INVALID_GRID&&m_grid != grid)//在移动到另外一个格子的情况下才计算
		{
			m_grid = grid;
			mTouchRole->setTouchGrid(m_grid);
			NOTIFICATION->postNotification(B_RoleAttackCostArea,mTouchRole);
		}
	}

	void BattleRoleLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (!mTouchRole)
			return;
		if (mTouchRole->getRoleObject())
			mTouchRole->getRoleObject()->showThis();
		mTouchRole->setInTouchState(false);
		lucencyActObject(false);
		mMoveRole->setVisible(false);
		NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
	}
	//@@应该是给武将一个目标，武将去自行判断位置是否可以移动过去，而不应该把武将的移动判断逻辑放在这个地方进行处理。
	void BattleRoleLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
	{
		mMoveRole->setVisible(false);
		NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
		lucencyActObject(false);
		m_grid = getTouchGrid(pTouch);
		mTouchRole->setCommandGrid(m_grid);
		mTouchRole->setInTouchState(false);
	}

	BaseRole* BattleRoleLayer::getAliveByMoveGrid( int pGrid )
	{
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(mMovetParent->getChildren(),obj)
		{
			MoveObject* tMoveObject = (MoveObject*)obj;
			for (auto tStandGrid:tMoveObject->grids)
			{
				if ( tStandGrid != pGrid )
					continue;
				return tMoveObject->getMoveAlive();
			}
		}
		return nullptr;
	}

	CCArray* BattleRoleLayer::getAlivesOb(int AliveType /*=AliveType_All*/)
	{
		CCArray* arr = mRoleNode->getChildren();
		CCArray* arrAlive = CCArray::create();
		for(int i = 0; i < int(arr->count());++i)
		{
			RoleObject* aliveOb = dynamic_cast<RoleObject*>(arr->objectAtIndex(i));
			if(!aliveOb || aliveOb == mMoveRole) 
				continue;
			if (AliveType == AliveType_All)
			{
				arrAlive->addObject(aliveOb);
			}else{
				if (aliveOb->getEnemy()&&AliveType==AliveType_Monster
					||!aliveOb->getEnemy()&&AliveType==AliveType_Hero)
				{
					arrAlive->addObject(aliveOb);
				}
			}
		}
		return arrAlive;
	}
	//0.4s震13次为基准,反馈过来的数据是它的倍数
	void BattleRoleLayer::LayerShake(CCObject* ob)
	{
		mRoleNode->stopAllActions();
		mRoleNode->setPosition(ccp(0,0));
		CCPoint p = mRoleNode->getPosition();
		mRoleNode->runAction(CCSequence::create(CCShake::create(0.4f,13.0f),CCPlace::create(p),NULL));
	}

	void BattleRoleLayer::clearAlivesPauseMark()
	{
		CCArray* arr = getAlivesOb();
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			act->setUserObject(nullptr);
		}
	}
	
	void BattleRoleLayer::heroWinAction(CCObject* ob)
	{
		CCArray* arr = getAlivesOb(AliveType_Hero);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			act->TurnStateTo(sStateCode::eVictoryState);
		}
	}

	void BattleRoleLayer::createBatchMonster( CCObject* ob )
	{
		CCArray* arr = mManage->getMonsters(true);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			BaseRole* alive = (BaseRole*)obj;
			if(alive->getAliveID() >= C_BatchMonst+mManage->getCurrBatch()*100)
				initActobject(alive);
		}
	}

	void BattleRoleLayer::changeLight( CCObject* ob )
	{
		bool pLight = ((CCBool*)ob)->getValue();
		m_LayerColor->setVisible(pLight);
		if (pLight)
		{
			LGPause(this);
		}else{
			LGResume(this);
			clearAlivesPauseMark();
		}
	}

	void BattleRoleLayer::SkillCreateRole( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		initActobject(tRole);
	}

	void BattleRoleLayer::changeTestState( CCObject* ob )
	{
		mtestState = !mtestState;
	}

	void BattleRoleLayer::roleStand( CCObject* ob )
	{
		CCArray* pRoleObjects = getAlivesOb();
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(pRoleObjects,obj)
		{
			RoleObject* tRoleObject = (RoleObject*)obj;
			tRoleObject->setMoveState(sStateCode::eNullState);
			tRoleObject->TurnStateTo(sStateCode::eStandState);
		}
	}
};