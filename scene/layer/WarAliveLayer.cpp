
#include "WarAliveLayer.h"
#include "model/DataCenter.h"
#include "Global.h"
#include "scene/AnimationManager.h"
#include "common/CommonFunction.h"
#include "warscene/ConstNum.h"
#include "warscene/SkillRange.h"
#include "tools/commonDef.h"
#include "common/color.h"
#include "Battle/RoleObject/RoleObject.h"
#include "warscene/MoveRule.h"
#include "Battle/EffectObject.h"
#include "tools/CCShake.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "warscene/CombatGuideManage.h"
#include "warscene/CombatGuideData.h"
#include "Battle/BattleScene/BattleScene.h"
#include "tools/ShowTexttip.h"
#include "warscene/ComBatLogic.h"
#include "warscene/SkillRange.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "warscene/CHeroSoundData.h"
#include "Battle/BattleMessage.h"
#include "Battle/MoveObject.h"
#include "Battle/BaseRole.h"
#include "Battle/BaseRoleData.h"
namespace BattleSpace{
	WarAliveLayer::WarAliveLayer()
		:m_TouchAlive(nullptr),m_grid(0),m_AliveNode(0),m_testState(true)
		,m_MoveActObject(nullptr),m_TouchAliveBtn(false),m_Manage(nullptr)
		,m_LayerColor(nullptr),m_MoveNode(nullptr),m_TouchOffs(0,0)
	{}
	WarAliveLayer::~WarAliveLayer()
	{
		m_TouchAlive = nullptr;
	}

	void WarAliveLayer::onEnter()
	{
		BaseLayer::onEnter();
		this->scheduleUpdate();
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAliveLayer::LayerShake),B_Shark,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAliveLayer::roleEntranceBattle),B_EntranceBattle,nullptr);
		NOTIFICATION->addObserver(this,callfuncO_selector(WarAliveLayer::changeTestState),B_ChangeMoveState,nullptr);
	}

	void WarAliveLayer::onExit()
	{
		CC_SAFE_RELEASE(m_MoveActObject->getAlive());
		this->unscheduleUpdate();
		removeEvent();
		NOTIFICATION->removeAllObservers(this);
		BaseLayer::onExit();
	}

	bool WarAliveLayer::init()
	{
		if (!BaseLayer::init())
			return false;
		this->setVisible(true);
		this->setAnchorPoint(ccp(0,0));
		this->setTouchPriority(AliveLayerPriority);
		this->setTouchEnabled(false);
		this->setIsShowBlack(false);
		m_MoveNode = CCNode::create();
		addChild(m_MoveNode);
		m_AliveNode = CCNode::create();
		addChild(m_AliveNode);
		m_Manage = DataCenter::sharedData()->getWar();
		m_map = DataCenter::sharedData()->getMap()->getCurrWarMap();

		createMoveTarget();
		createLayerColor();
		createActObjects();
		return true;
	}

	void WarAliveLayer::createMoveTarget()
	{
		BaseRole* alive = BaseRole::create();
		alive->retain();
		m_MoveActObject = RoleObject::create();
		m_MoveActObject->setAlive(alive);
		m_MoveActObject->setVisible(false);
		addChild(m_MoveActObject); 
	}

	void WarAliveLayer::createLayerColor()
	{
		m_LayerColor = CCLayerColor::create(ccc4(0,0,0,200));
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		m_LayerColor->setContentSize(size*4);
		m_LayerColor->setPosition(ccp(-size.width, -size.height));
		m_LayerColor->setVisible(false);
		m_AliveNode->addChild(m_LayerColor);
	}

	void WarAliveLayer::createActObjects()
	{
		const Members* map_Alive = m_Manage->getMembers();
		for (auto tPair : *map_Alive)
		{
			if (tPair.second->getHp()>0 && tPair.second->getEnemy())
				initActobject(tPair.second);
		}
		initActobject(m_Manage->getAliveByGrid(C_CAPTAINSTAND));
	}

	void WarAliveLayer::AliveObEffect(RoleObject* aliveOb,int createType/*=DefaultCreat*/)
	{
		switch (createType)
		{
		case DefaultCreat:
			{
				aliveOb->setDropItem(aliveOb->getAlive()->getBaseData()->getRoleDrop());	
			}break;
		case SceneTrap:
			{
				EffectObject* eff = EffectObject::create("304");
				eff->setEffAnchorPoint(0.5f,0.5f);
				eff->setPosition(ccp(0,GRID_HEIGHT*1.0f));
				aliveOb->addChild(eff);
				aliveOb->setDropItem(aliveOb->getAlive()->getBaseData()->getRoleDrop());
				PlayEffectSound(SFX_517);
			}break;
		}
	}

	void WarAliveLayer::AddActToGrid(RoleObject* pAliveOb,int grid)
	{
		BaseRole* alive = pAliveOb->getAlive();
		if (alive->getDelaytime()>0)grid = 0;
		if (grid == INVALID_GRID&&pAliveOb->getEnemy())
		{
			grid = CCRANDOM_0_1()*3+1;
			alive->setGridIndex(grid);
		}
		pAliveOb->countOffs(m_map->getPoint(grid));
		if(pAliveOb->getParent() == nullptr)
		{
			m_AliveNode->addChild(pAliveOb);
			pAliveOb->initMoveObject(m_MoveNode);
		}
	}
	//绘制战场武将
	void WarAliveLayer::initActobject(BaseRole* alive,int createType)
	{
		if (alive->getRoleObject())
		{
			CCLOG("[ *TIPS ] WarAliveLayer::createAlive alive Actobject Repeat");
			return;
		}
		RoleObject* aliveOb = RoleObject::create();							//创建显示的对象
		aliveOb->setAlive(alive);											//显示对象与逻辑对象绑定
		alive->setRoleLayer(this);
		AliveObEffect(aliveOb,createType);
		AddActToGrid(aliveOb,alive->getGridIndex());
	}
	void WarAliveLayer::addEvent(){ this->setTouchEnabled(true); }
	void WarAliveLayer::removeEvent(){ this->setTouchEnabled(false); }

	void WarAliveLayer::removeMessage()
	{
		ccTouchCancelled(nullptr,nullptr);
		removeEvent();
		addEvent();
	}
	//不断刷新武将的Z轴值
	void WarAliveLayer::update(float delta)
	{
		CCArray* arr = m_AliveNode->getChildren();
		CCObject* object;
		CCARRAY_FOREACH(arr,object)
		{
			RoleObject* act = dynamic_cast<RoleObject*>(object);
			if(!act || object == m_MoveActObject) continue;
			int zorder = -act->getPositionY();
			if(act->getParent()) 
				act->getParent()->reorderChild(act,zorder);	//z排序
			act->updatePosition(delta);
		}
		if( m_MoveActObject->getParent() ) 
			m_MoveActObject->getParent()->reorderChild(m_MoveActObject,200);			//刷新移动武将Z值排序
	}
	//初始化触摸武将存储的信息。
	void WarAliveLayer::roleEntranceBattle(CCObject* ob)
	{
		BaseRole* tRole = (BaseRole*)ob;
		initActobject(tRole);
		initTouchAlive(tRole);
		m_TouchAliveBtn = true;
	}

	void WarAliveLayer::initMoveActObject( RoleObject* aliveOb )
	{
		if (m_MoveActObject->getModel() != aliveOb->getModel())
		{
			if (m_MoveActObject->getArmature())
			{
				m_MoveActObject->getArmature()->removeFromParentAndCleanup(true);
				m_MoveActObject->setArmature(nullptr);
			}
			m_MoveActObject->setModel(aliveOb->getModel());	//移动的目标是模型是被点击的目标模型'
			m_MoveActObject->setoffs(aliveOb->getoffs());		//武将原来相对于格子的偏移量
			m_MoveActObject->getBody()->setScale(aliveOb->getBody()->getScale());
		}
		if (aliveOb->getAlive()->getGridIndex())
		{
			if(aliveOb->getMoveObject())
				m_MoveActObject->setPosition(aliveOb->getMoveObject()->getPosition());
		}else{
			m_MoveActObject->setPosition(ccp(-500,100));
		}
		m_MoveActObject->setVisible(true);
	}

	void WarAliveLayer::lucencyActObject(bool lucency)
	{
		CCArray*ob_Arr = getAlivesOb(AliveType_Hero);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(ob_Arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			BaseRole* t_alive = act->getAlive();	
			if (t_alive->getCaptain())continue;
			setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),255);
			if (lucency)
				setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),100);
		}
	}

	void WarAliveLayer::initTouchAlive(BaseRole* alive)
	{
		m_TouchAlive = alive;
		RoleObject* aliveOb = alive->getRoleObject();										//指针复制，复制被触摸武将
		alive->setTouchGrid(alive->getGridIndex());
		aliveOb->showThis();
		NOTIFICATION->postNotification(B_RoleAttackCostArea, alive);
		initMoveActObject(aliveOb);
		if (!alive->getBattle())								//上阵武将才做透明处理
			return;
		lucencyActObject(true);
	}

	bool WarAliveLayer::touchInAlive( int grid,CCPoint& p )
	{
		vector<BaseRole*>* Vec = m_Manage->getVecHeros();
		for (auto i : *Vec)
		{
			if (i->getCallType()!=CommonType)continue;
			MoveObject* mo = i->getMoveObject();
			if (!mo)continue;
			for (auto j :mo->grids)
			{
				if (j!=grid)																//当触摸在mo的grid上就没有问题了,将当前触摸的点求出如果触摸在grid上的偏移量来
					continue;
				CCPoint upoint = mo->getParent()->convertToWorldSpace(mo->getPosition());	//转化为世界坐标进行偏移量处理
				m_TouchOffs = upoint - p;													//加减号被重载了,出触摸偏移量
				initTouchAlive(i);
				return true;
			}
		}
		return false;
	}

	bool WarAliveLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (m_TouchAliveBtn)
		{
			m_TouchAliveBtn = false;
			return true;
		}
		CCPoint p = this->convertToNodeSpace(pTouch->getLocation());
		int grid = m_map->getGridIndex(p);
		return touchInAlive(grid,pTouch->getLocation());
	}

	int WarAliveLayer::getTouchGrid( CCTouch* pTouch )
	{
		CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs;
		return m_map->getGridIndex(p-m_MoveActObject->getoffs());								//移动目标所站的实际格子要减去偏移量
	}

	void WarAliveLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (!m_MoveActObject->isVisible())
			return;
		CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs;
		int grid = getTouchGrid(pTouch);
		m_MoveActObject->setPosition(p);
		if (grid != INVALID_GRID&&m_grid != grid)//在移动到另外一个格子的情况下才计算
		{
			m_grid = grid;
			m_TouchAlive->setTouchGrid(m_grid);
			NOTIFICATION->postNotification(B_RoleAttackCostArea,m_TouchAlive);
		}
	}

	void WarAliveLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (!m_TouchAlive)
			return;
		if (m_TouchAlive->getRoleObject())
			m_TouchAlive->getRoleObject()->showThis();
		lucencyActObject(false);
		m_MoveActObject->setVisible(false);
		NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
	}
	//@@应该是给武将一个目标，武将去自行判断位置是否可以移动过去，而不应该把武将的移动判断逻辑放在这个地方进行处理。
	void WarAliveLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
	{
		m_MoveActObject->setVisible(false);
		NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
		lucencyActObject(false);
		m_grid = getTouchGrid(pTouch);
		m_TouchAlive->setTouchEndGrid(m_grid);
	}

	BaseRole* WarAliveLayer::getAliveByMoveGrid( int pGrid )
	{
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(m_MoveNode->getChildren(),obj)
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

	CCArray* WarAliveLayer::getAlivesOb(int AliveType /*=AliveType_All*/)
	{
		CCArray* arr = m_AliveNode->getChildren();
		CCArray* arrAlive = CCArray::create();
		for(int i = 0; i < int(arr->count());++i)
		{
			RoleObject* aliveOb = dynamic_cast<RoleObject*>(arr->objectAtIndex(i));
			if(!aliveOb || aliveOb == m_MoveActObject) 
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
	void WarAliveLayer::LayerShake(CCObject* ob)
	{
		m_AliveNode->stopAllActions();
		m_AliveNode->setPosition(ccp(0,0));
		CCPoint p = m_AliveNode->getPosition();
		m_AliveNode->runAction(CCSequence::create(CCShake::create(0.4f,13.0f),CCPlace::create(p),NULL));
	}

	void WarAliveLayer::clearAlivesPauseMark()
	{
		CCArray* arr = getAlivesOb();
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			act->setUserObject(nullptr);
		}
	}

	void WarAliveLayer::heroWinAction()
	{
		CCArray* arr = getAlivesOb(AliveType_Hero);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			RoleObject* act = (RoleObject*)obj;
			act->TurnStateTo(E_StateCode::eVictoryState);
		}
	}

	void WarAliveLayer::createBatchMonster( int batchNumber )
	{
		CCArray* arr =m_Manage->getAlivesByCamp(true,true);
		CCObject* obj = nullptr;
		CCARRAY_FOREACH(arr,obj)
		{
			BaseRole* alive = (BaseRole*)obj;
			if(alive->getAliveID() >= C_BatchMonst+batchNumber*100)
				initActobject(alive,SceneTrap);
		}
	}

	void WarAliveLayer::roleWantIntoBattle( BaseRole* pRole )
	{
		AliveObEffect(pRole->getRoleObject());
		AddActToGrid(pRole->getRoleObject(),pRole->getGridIndex());
		pRole->roleIntoBattle();
	}

	void WarAliveLayer::changeLight( bool pLight )
	{
		m_LayerColor->setVisible(pLight);
		if (pLight)
		{
			LGPause(this);
		}else{
			LGResume(this);
			clearAlivesPauseMark();
		}
	}

	void WarAliveLayer::SkillCreateRole( CCObject* ob )
	{
		BaseRole* tRole = (BaseRole*)ob;
		initActobject(tRole,SceneTrap);
	}

	void WarAliveLayer::changeTestState( CCObject* ob )
	{
		m_testState = !m_testState;
	}
};