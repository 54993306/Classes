
#include "WarAliveLayer.h"
#include "model/DataCenter.h"
#include "Global.h"
#include "scene/AnimationManager.h"
#include "common/CommonFunction.h"
#include "warscene/ConstNum.h"
#include "warscene/SkillRange.h"
#include "tools/commonDef.h"
#include "common/color.h"
#include "scene/alive/AliveDefine.h"
#include "warscene/MoveRule.h"
#include "scene/effect/EffectObject.h"
#include "tools/CCShake.h"
#include "model/WarManager.h"
#include "model/MapManager.h"
#include "warscene/CombatGuideManage.h"
#include "warscene/CombatGuideData.h"
#include "scene/WarScene.h"
#include "tools/ShowTexttip.h"
#include "warscene/ComBatLogic.h"
#include "warscene/SkillRange.h"
#include "common/ShaderDataHelper.h"
#include "common/CGameSound.h"
#include "Resources.h"
#include "warscene/CHeroSoundData.h"
#include "Battle/BattleMessage.h"
WarAliveLayer::WarAliveLayer()
	:m_TouchAlive(nullptr),m_grid(0),m_AliveNode(0)
	,m_MoveActObject(nullptr),m_TouchAliveBtn(false),m_Manage(nullptr)
	,m_LayerColor(nullptr),m_MoveNode(nullptr),m_TouchOffs(0,0)
{}
WarAliveLayer::~WarAliveLayer()
{
	m_TouchAlive = nullptr;
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
	createMoveTarget();
	m_MoveNode = CCNode::create();
	addChild(m_MoveNode);
	m_AliveNode = CCNode::create();
	addChild(m_AliveNode);
	m_Manage = DataCenter::sharedData()->getWar();
	m_map = DataCenter::sharedData()->getMap()->getCurrWarMap();
	createActObjects();
	return true;
}

void WarAliveLayer::createMoveTarget()
{
	WarAlive* alive = WarAlive::create();
	alive->retain();
	m_MoveActObject = ActObject::create();
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
	CCArray* arr = m_Manage->getAlives(true);
	CCObject* obj =nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if (!alive->getEnemy() && !alive->getCaptain())
			continue;
		initActobject(alive);
	}
}

void WarAliveLayer::AliveObEffect(ActObject* aliveOb,int createType/*=DefaultCreat*/)
{
	switch (createType)
	{
	case DefaultCreat:
		{
			aliveOb->setDropItem(aliveOb->getAlive()->role->hasitem);	
		}break;
	case SceneTrap:
		{
			EffectObject* eff = EffectObject::create("304");
			eff->setEffAnchorPoint(0.5f,0.5f);
			eff->setPosition(ccp(0,GRID_HEIGHT*1.0f));
			aliveOb->addChild(eff);
			aliveOb->setDropItem(aliveOb->getAlive()->role->hasitem);
			PlayEffectSound(SFX_517);
		}break;
	}
}

void WarAliveLayer::monsterSoleSprite( ActObject* aliveOb )
{
	WarAlive* alive = aliveOb->getAlive();
	if (!alive->getEnemy())
		return;
	CCSprite* ef = nullptr;
	for (int i=0;i<alive->role->row;i++)
		for (int j =0;j<alive->role->col;j++)
		{
			ef = CCSprite::create("warScene/fanglandingwei.png");
			ef->setPosition(ccp(-aliveOb->getoffs().x+(j*(GRID_WIDTH+C_GRIDOFFSET_X)),-aliveOb->getoffs().y-(i*(GRID_HEIGHT+C_GRIDOFFSET_Y))));
			//aliveOb->addChild(ef,-1);
		}
}

void WarAliveLayer::initActObjectOffs(ActObject* aliveOb,int grid)
{
	WarAlive* alive = aliveOb->getAlive();
	if (alive->role->row>1)
	{
		if (alive->role->row>2)
		{
			aliveOb->setPosition(ccpAdd(aliveOb->getPosition(),ccp(0,-GRID_HEIGHT/1.5f*alive->role->row)));	
		}else{
			aliveOb->setPosition(ccpAdd(aliveOb->getPosition(),ccp(0,-GRID_HEIGHT/3*alive->role->row)));
		}
	}
	if (alive->role->col>1&&!alive->getCaptain())
		aliveOb->setPosition(ccpAdd(aliveOb->getPosition(),ccp(GRID_WIDTH/4*alive->role->col,0)));
	aliveOb->setoffs(aliveOb->getPosition()-m_map->getPoint(grid));		//算出武将偏移实际站位格子的偏移量
}

void WarAliveLayer::initActObjectPosition( ActObject* aliveOb,int grid )
{
	if (grid == INVALID_GRID&&aliveOb->getEnemy())
	{
		grid = CCRANDOM_0_1()*3+1;
		aliveOb->getAlive()->setGridIndex(grid);
		aliveOb->setPosition(m_map->getPoint(grid));
	}else{
		aliveOb->setPosition(m_map->getPoint(grid));
	}
}

void WarAliveLayer::creatMoveObject( WarAlive* alive )
{
	if (!alive->getEnemy()&&!alive->getCaptain())
	{
		MoveObj* Moveobj = MoveObj::create();
		Moveobj->setAlive(alive);
		Moveobj->setActObject(alive->getActObject());
		Moveobj->setgrid(alive->getGridIndex());
		alive->setMoveObj(Moveobj);
		alive->getActObject()->setMoveObj(Moveobj);
	}else{
		alive->setAliveStat(INVINCIBLE);
		alive->getActObject()->TurnStateTo(Start_Index);
	}
}

void WarAliveLayer::AddActToGrid(ActObject* aliveOb,int grid)
{
	WarAlive* alive = aliveOb->getAlive();
	if (alive->getDelaytime()>0)
		grid = 0;
	initActObjectPosition(aliveOb,grid);
	initActObjectOffs(aliveOb,grid);
	monsterSoleSprite(aliveOb);
	if(aliveOb->getParent() == nullptr)
	{
		m_AliveNode->addChild(aliveOb);
		if (alive->getMoveObj())
		{
			alive->getMoveObj()->initMoveSprite();
			m_MoveNode->addChild(alive->getMoveObj());
		}
	}
}
//绘制战场武将
void WarAliveLayer::initActobject(WarAlive* alive,int createType)
{
	if (alive->getActObject())
	{
		CCLOG("[ *TIPS ] WarAliveLayer::createAlive alive Actobject Repeat");
		return;
	}
	ActObject* aliveOb = ActObject::create();							//创建显示的对象
	aliveOb->setAlive(alive);											//显示对象与逻辑对象绑定
	aliveOb->setEnemy(alive->getEnemy());
	aliveOb->setModel(alive->getModel());
	aliveOb->setHp(nullptr); 
	aliveOb->getBody()->setScale(alive->getZoom());
	alive->setActObject(aliveOb);										//逻辑对象与显示对象绑定
	if (alive->getCloaking())											//潜行怪物处理
		((CCArmature*)aliveOb->getArmature())->setOpacity(125);
	AliveObEffect(aliveOb,createType);
	creatMoveObject(alive);
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

void WarAliveLayer::onEnter()
{
	BaseLayer::onEnter();
	this->scheduleUpdate();
	NOTIFICATION->addObserver(this,callfuncO_selector(WarAliveLayer::LayerShake),B_Shark,nullptr);
	NOTIFICATION->addObserver(this,callfuncO_selector(WarAliveLayer::aliveEntranceBattle),B_EntranceBattle,nullptr);
}

void WarAliveLayer::onExit()
{
	CC_SAFE_RELEASE(m_MoveActObject->getAlive());
	this->unscheduleUpdate();
	removeEvent();
	NOTIFICATION->removeAllObservers(this);
	BaseLayer::onExit();
}
//不断刷新武将的Z轴值
void WarAliveLayer::update(float delta)
{
	CCArray* arr = m_AliveNode->getChildren();
	CCObject* object;
	CCARRAY_FOREACH(arr,object)
	{
		ActObject* act = dynamic_cast<ActObject*>(object);
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
void WarAliveLayer::aliveEntranceBattle(CCObject* ob)
{
	WarAlive* alive = (WarAlive*)ob;
	initActobject(alive);
	TouchAlive(alive);
	m_TouchAliveBtn = true;
}

void WarAliveLayer::initMoveActObject( ActObject* aliveOb )
{
	m_MoveActObject->setModel(aliveOb->getModel());	//移动的目标是模型是被点击的目标模型
	m_MoveActObject->TurnStateTo(Stand_Index);
	m_MoveActObject->setoffs(aliveOb->getoffs());		//武将原来相对于格子的偏移量
	if (aliveOb->getAlive()->getGridIndex())
	{
		if(aliveOb->getMoveObj())
			m_MoveActObject->setPosition(aliveOb->getMoveObj()->getPosition());
	}else{
		m_MoveActObject->setPosition(ccp(-500,100));
	}
	m_MoveActObject->getBody()->setScale(aliveOb->getBody()->getScale());;
	m_MoveActObject->setVisible(true);
}


void WarAliveLayer::lucencyActObject(bool lucency)
{
	CCArray*ob_Arr = getAlivesOb(AliveType_Hero);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(ob_Arr,obj)
	{
		ActObject* act = (ActObject*)obj;
		WarAlive* t_alive = act->getAlive();	
		if (t_alive->getCaptain())continue;
		setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),255);
		if (lucency)
			setEnableRecursiveCascading(act->getArmature(),true,ccc3(255,255,255),100);
	}
}

void WarAliveLayer::TouchAlive(WarAlive* alive)
{
	m_TouchAlive = alive;
	ActObject* aliveOb = alive->getActObject();										//指针复制，复制被触摸武将
	alive->setTouchGrid(alive->getGridIndex());
	aliveOb->getHp()->setVisible(true);
	NOTIFICATION->postNotification(B_RoleAttackCostArea, alive);
	initMoveActObject(aliveOb);
	if (!alive->getBattle())								//上阵武将才做透明处理
		return;
	lucencyActObject(true);
}

bool WarAliveLayer::touchInAlive( int grid,CCPoint& p )
{
	vector<WarAlive*>* Vec = m_Manage->getVecHeros();
	for (auto i : *Vec)
	{
		if (i->getCallType()!=CommonType)continue;
		MoveObj* mo = i->getMoveObj();
		if (!mo)continue;
		for (auto j :mo->grids)
		{
			if (j!=grid)																//当触摸在mo的grid上就没有问题了,将当前触摸的点求出如果触摸在grid上的偏移量来
				continue;
			CCPoint upoint = mo->getParent()->convertToWorldSpace(mo->getPosition());	//转化为世界坐标进行偏移量处理
			m_TouchOffs = upoint - p;													//加减号被重载了,出触摸偏移量
			TouchAlive(i);
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

void WarAliveLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if (!m_MoveActObject->isVisible())
		return;
	CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs;
	m_MoveActObject->setPosition(p);
	int grid = m_map->getGridIndex(p-m_MoveActObject->getoffs());								//移动目标所站的实际格子要减去偏移量
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
	if (m_TouchAlive->getActObject())
	{
		m_TouchAlive->getActObject()->setVisible(true);
		m_TouchAlive->getActObject()->getHp()->showHp(nullptr);
	}
	lucencyActObject(false);
	m_MoveActObject->setVisible(false);
	NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
}

void WarAliveLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	m_MoveActObject->setVisible(false);
	NOTIFICATION->postNotification(B_CancelCostArea,nullptr);
	CCPoint p = convertToNodeSpace(pTouch->getLocation())+m_TouchOffs-m_MoveActObject->getoffs();		//触摸点传递下来
	lucencyActObject(false);
	MoveAliveToGrid(m_TouchAlive,m_map->getGridIndex(p));
}

bool WarAliveLayer::moveGuide( int grid,bool nextStep )
{
	if (DataCenter::sharedData()->getCombatGuideMg()->IsGuide())					//引导判断
	{
		CombatGuideStep* step = DataCenter::sharedData()->getCombatGuideMg()->getCurrStep();
		if (step->getType()==AliveMove_Type || step->getType() == CallAalive_Type)
		{
			if (grid != step->getFinishgrid())
				return true;								//固定格子才算完成(在移动区域内即算完成)
			if (nextStep)
				DataCenter::sharedData()->getCombatGuideMg()->NextStep();
		}
	}
	return false;
}

void WarAliveLayer::MoveAliveToGrid( WarAlive* alive,int grid )
{
	if (!alive->getMove() || 
		!alive->getActObject() || 
		!alive->getMoveObj()	||
		grid==alive->getMoveObj()->getgrid())
		return;
	alive->getActObject()->setVisible(true);
	alive->getActObject()->getHp()->showHp(nullptr);
	vector<int>* vec = m_Manage->getMoveVec();
	if (std::find(vec->begin(),vec->end(),grid) == vec->end())
		return;
	if (WorldBossJudge(alive,grid))
		return ;
	if (moveGuide(grid,false))
		return;
	if(JudgeMove(alive,grid))								//当前位置是否可以放置英雄
	{
		alive->getMoveObj()->setgrid(grid);
		alive->getActObject()->setMoveState(Walk_Index);
	}else{
		return;	
	}
		
	if (moveGuide(grid,true))
		return;
	alive->setAIState(false);
}

bool WarAliveLayer::WorldBossJudge( WarAlive* alive,int grid )
{
	if (!DataCenter::sharedData()->getWar()->getNormal() && grid < 92)		//精英关卡创建武将只能右半屏
		return true;
	if (DataCenter::sharedData()->getWar()->getWorldBoss())
	{
		if (alive->getMoveObj()->getgrid() > 108 && grid < 108)				//写死的格子数
		{
			return true;
		}else if (alive->getMoveObj()->getgrid() < 80 && grid > 80)
		{
			return true;
		}
	}
	return false;
}


bool WarAliveLayer::moreAliveJudge( map<int , GridMoveObj>& MapMove,MoveObj* mObj )
{
	map<int,WarAlive*>MoveMap;
	for (auto i:MapMove)
	{
		int offs = i.first - i.second.first;					//目标格子和当前格子偏移量
		WarAlive* pAlive = i.second.second->getAlive();			//目标位置武将
		vector<int> pVec;
		for (auto j : i.second.second->grids)					//grids排序过的
		{									
			MoveObj* pObj = getMoveByGrid(j+offs);
			if (pObj&&pObj != mObj)								//如果被替换的武将移动过去的位置有其他人则不可替换位置
				return false;
			pVec.push_back(j+offs);
		}
		if (!BorderJudge(pAlive,pVec))
			return false;
		MoveMap[pVec.at(0)] = pAlive;
	}
	setMapAliveGrid(MoveMap);
	return true;
}

bool WarAliveLayer::swappingGrid( WarAlive* alive,vector<int>& VGrid )
{
	MoveObj* mObj = alive->getMoveObj();
	map<int , GridMoveObj> MapMove;								//pair<int ,MoveObj*> GridMoveObj;
	for (int i=0;i<VGrid.size();i++)
	{
		MoveObj* pObj = getMoveByGrid(VGrid.at(i));	
		if (!pObj||pObj==mObj)									
			continue;
		if (alive->getBattle())
		{
			GridMoveObj pairObj = make_pair(VGrid.at(i),pObj);	//几号格子存在其他我方武将
			MapMove[mObj->grids.at(i)] = pairObj;				//我方几号格子对应其他武将
		}else{
			if (DataCenter::sharedData()->getCombatGuideMg()->IsGuide())
			{
				CombatGuideStep* step = DataCenter::sharedData()->getCombatGuideMg()->getCurrStep();
				if (!step->getReset())
					return false;								//重置状态我方未上阵可替换其他武将
			}else{
				if (pObj->getAlive()->getCallType() == CommonType)
					return false;
			}
		}
	}
	return moreAliveJudge(MapMove,mObj);
}

void WarAliveLayer::setMapAliveGrid( map<int,WarAlive*>& MoveMap )
{
	for (auto i:MoveMap)
	{
		if (i.second->getHp()<=0||!i.second->getBattle())
			continue;
		i.second->getMoveObj()->setgrid(i.first);
		i.second->getActObject()->setMoveState(Walk_Index);
	}
}

//判断武将是否可以移动的方法
bool WarAliveLayer::JudgeMove(WarAlive*alive,int grid)
{
	vector<int> VGrid ;
	for (int i=0;i<alive->role->row;i++)
		for (int j =0;j<alive->role->col;j++)
			VGrid.push_back(grid+j*C_GRID_ROW+i);
	sort(VGrid.begin(),VGrid.end());
	if (!BorderJudge(alive,VGrid))
		return false;
	if (alive->getCallType() != CommonType)
		return true;
	if (!swappingGrid(alive,VGrid))
		return false;
	return true;
}
//边界判断
bool WarAliveLayer::BorderJudge( WarAlive* alive,vector<int>&Vec )
{
	if (!Vec.size())
		return false;
	for (auto i : Vec)									//主帅位置不可替换
		if (i>=C_CAPTAINGRID||i<C_GRID_ROW+C_BEGINGRID)	//我方武将边缘处理
			return false;
	int row = Vec.at(0)%C_GRID_ROW;						//最小格子的站位
	if (row+alive->role->row>C_GRID_ROW)				//武将所占格子,不能超出地图外
		return false;
	return true;
}

MoveObj* WarAliveLayer::getMoveByGrid( int grid )
{
	CCArray* arr = m_MoveNode->getChildren();
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		MoveObj* mo = (MoveObj*)obj;
		if (mo->getAlive()->getCallType() != CommonType)
			continue;
		for (auto i:mo->grids)
		{
			if (i==grid)
				return mo;
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
		ActObject* aliveOb = dynamic_cast<ActObject*>(arr->objectAtIndex(i));
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
		ActObject* act = (ActObject*)obj;
		act->setUserObject(nullptr);
	}
}

void WarAliveLayer::heroWinAction()
{
	CCArray* arr = getAlivesOb(AliveType_Hero);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		ActObject* act = (ActObject*)obj;
		act->TurnStateTo(victory_Index);
	}
}

void WarAliveLayer::createBatchMonster( int batchNumber )
{
	CCArray* arr =m_Manage->getMonsts(true);
	CCObject* obj = nullptr;
	CCARRAY_FOREACH(arr,obj)
	{
		WarAlive* alive = (WarAlive*)obj;
		if(alive->getAliveID() >= C_BatchMonst+batchNumber*100)
			initActobject(alive,SceneTrap);
	}
}