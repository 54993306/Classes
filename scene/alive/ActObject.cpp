
#include "scene/alive/ActObject.h"
#include "tools/StringUtil.h"
#include "model/WarManager.h"
#include "model/DataCenter.h"
#include "model/BuffManage.h"
#include "Battle/BattleRole.h"
#include "model/MapManager.h"
#include "warscene/ConstNum.h"
#include "warscene/EffectData.h"
#include "warscene/CombatTask.h"
#include "scene/alive/HPObject.h"
#include "common/CommonFunction.h"
#include "scene/AnimationManager.h"
#include "scene/effect/EffectObject.h"
#include "warscene/ArmatureEventDataMgr.h"
#include "common/ShaderDataHelper.h"
#include "Battle/BattleMessage.h"
#include "warscene/BattleTools.h"
#include "Battle/MoveObject.h"
#include "Battle/RoleBaseData.h"
//#include <spine/AnimationState.h>
ActObject::ActObject()
	:m_MapData(nullptr),m_armatureEventData(nullptr), m_lastFrame(-1),m_Reset(false)
{}

ActObject::~ActObject()
{
	NOTIFICATION->removeAllObservers(this);
}
bool ActObject::init()
{
	if (!AliveObject::init())
		return false;
	m_MapData = DataCenter::sharedData()->getMap()->getCurrWarMap();
	m_Manage = DataCenter::sharedData()->getWar();
	return true;
}

void ActObject::setAlive( WarAlive* var )
{
	AliveObject::setAlive(var);
	initStateManage(); 
	if (!m_Alive->getAliveID())
		return;															//挂在层显示拖动的目标(可以单独抽出一个方法来)
	m_Alive->setActObject(this);										//逻辑对象与显示对象绑定
	setEnemy(var->getEnemy());
	setModel(m_Alive->getModel());
	setHp(nullptr);
	getBody()->setScale(m_Alive->getZoom());
	if (m_Alive->getCloaking())											//潜行怪物处理
		getArmature()->setOpacity(125);
	if (m_Enemy)
	{
		m_Alive->setAliveStat(INVINCIBLE);
		TurnStateTo(Start_Index);
	}
}

void ActObject::initStateManage()
{
	GameStateManager* stateManage = GameStateManager::create();						//继承实现状态管理器
	GameStateTransition* transition = GameStateTransition::create();				//继承实现状态切换列表对象
	this->setStateManager(stateManage);												//设置状态机的状态管理器
	this->setTransition(transition);												//设置状态切换对象
	transition->RoleTransition(m_Alive->getEnemy());
}

void ActObject::setModel(int pModel)//设置人物对应模型
{
	m_Model = pModel;
	if ( m_Manage->isSpine(m_Model) )
	{
		m_IsSpine = true;
		initSpineModel();
	}else{
		m_IsSpine = false;
		initCocosModel();
	}
	m_Armature->setPosition(ccp(0,-GRID_HEIGHT/2));
	m_Body->addChild(m_Armature);
	this->getStateManage()->initState(this);							//设置人物的初始动作为站立也可为其他						
}

void ActObject::setActionKey(string ActionKey)//设置人物对应动作(动作切换)然后刷新人物运行动作
{
	m_ActionKey = ActionKey;
	if (m_ActionKey == Walk_Action)			//我方突然中断整个战斗流程的情况(玩家操纵拖动武将)
		toWlkActionDispose();
	if (m_IsSpine)
	{
		setSpineAction();
	}else{
		setCocosAction();
	}
}

void ActObject::toWlkActionDispose()
{
	NOTIFICATION->postNotification(B_CancelDrawAttackArea,m_Alive);//取消绘制攻击范围(针对性的取消绘制)
	if (m_Alive->getNorAtk()&&!m_Alive->getSpeAtk()&&!m_Alive->getCriAtk())
	{
		m_Alive->ResetAttackState();
		m_Alive->setNorAtk(true);					//普通攻击情况下,移动不需重置攻击时间
	}else{
		m_Alive->ResetAttackState();
	}
}

void ActObject::setSpineAction()
{
	SkeletonAnimation* Skeleton = (SkeletonAnimation*)m_Armature;
	hideSpineEffect();
	for (int i = 0;i<Skeleton->state->data->skeletonData->animationsCount;i++)
	{
		if ( !strcmp(Skeleton->state->data->skeletonData->animations[i]->name,m_ActionKey.c_str()))
		{
			if ( !strcmp(m_ActionKey.c_str(),Die_Action))
				((SkeletonAnimation*)m_Armature)->setAnimation(0,m_ActionKey.c_str(),false);
			else
				((SkeletonAnimation*)m_Armature)->setAnimation(0,m_ActionKey.c_str(),true);
			setSpineEffectAction();
			return;
		}
	}
	lostAction();
}

void ActObject::setSpineEffectAction()
{
	if ( !m_Skeleton)
		return;
	for (int i = 0;i<m_Skeleton->skeleton->data->animationsCount;i++)
	{
		if ( !strcmp(m_Skeleton->skeleton->data->animations[i]->name,m_ActionKey.c_str()))
		{
			m_Skeleton->setVisible(true);
			m_Skeleton->setAnimation(0,m_ActionKey.c_str(),false);
			return;
		}
	}
	hideSpineEffect();
}

void ActObject::hideSpineEffect()
{
	if (!m_Skeleton)
		return;
	m_Skeleton->setToSetupPose();
	m_Skeleton->setVisible(false);
}

void ActObject::lostAction()
{
	if (m_ActionKey.compare(Start_Action)==0)
	{
		m_Alive->setAliveStat(COMMONSTATE);
	}else if ( !strcmp(m_ActionKey.c_str(),Stand_Action))
	{
		CCLOG("[ **ERROR ] ActObject::lostAction Lost StandAction");
		return;
	}else{
		CCLOG("[ *ERROR ] ActObject::lostAction Lost Action=[%s],model=[%d]",m_ActionKey.c_str(),m_Model);
	}
	this->TurnStateTo(Stand_Index);
}

void ActObject::setCocosAction()
{
	CCArmature* Armature = (CCArmature*) m_Armature;
	CCMovementData* MovementData = Armature->getAnimation()->getAnimationData()->getMovement(m_ActionKey.c_str());
	if (MovementData)				//判断动作是否存在
	{
		Armature->getAnimation()->play(m_ActionKey.c_str(),0.01f);
	}else{
		lostAction();
	}
}

void ActObject::initSpineEffect()
{
	char efName[60] = {0};
	sprintf(efName,"%d_texiao",m_Model);
	SpData* EFdata = m_Manage->getSpineData(efName);
	if (EFdata)
	{
		m_Skeleton = SkeletonAnimation::createWithData(EFdata->first);
		m_Skeleton->setPosition(ccp(0,-GRID_HEIGHT/2));
		m_Body->addChild(m_Skeleton,1);
	}else{
		CCLOG("[ *TIPS ]  ActObject::setModel Spine Model=%d IS NULL",m_Model); 
	}
}

void ActObject::initSpineModel()
{
	SpData* data = m_Manage->getSpineData(ToString(m_Model));
	if (!data)
	{
		data = m_Manage->getSpineData("146");
		CCLOG("[ *ERROR ]  ActObject::setModel Spine Model=%d IS NULL",m_Model); 
	}
	SkeletonAnimation*  Animation = SkeletonAnimation::createWithData(data->first);
	CCAssert(Animation,"ActObject::setModel Spine NULL");
	Animation->endListener = std::bind(&ActObject::SpineActionEnd,this,std::placeholders::_1);
	Animation->completeListener = std::bind(&ActObject::SpineComplete,this,std::placeholders::_1,std::placeholders::_2);
	Animation->eventListener = std::bind(&ActObject::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
	m_Armature = Animation;
	initSpineEffect();
}

void ActObject::initCocosModel()
{
	CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(m_Model));
	if (!animationData)													//模型容错性处理
	{
		CCLOG("[ *ERROR ]  ActObject::setModel Animation Model=%d IS NULL",m_Model); 
		m_Model = 516;
	}	
	CCArmature* Armature = CCArmature::create(ToString(m_Model));
	Armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ActObject::AnimationEvent));		//动作结束回调
	m_Armature = Armature;
	m_armatureEventData = m_Manage->getArmatureDataMgr()->getArmatureEventData(m_Model);
}
//动作结束回调
void ActObject::AnimationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	std::string ActionName = movementID;
	ActionName = strRemoveSpace(ActionName);						//去空格
	if (movementType == LOOP_COMPLETE || movementType == COMPLETE)	//动作结束的标记(分为是否循环动作两种)
		ActionEnd(ActionName.c_str());
}
//在end里面set动作会崩掉所以引入了Complete方法
void ActObject::SpineActionEnd( int trackIndex )
{
	spTrackEntry*TrackEntry = ((SkeletonAnimation*)m_Armature)->getCurrent(trackIndex);
	if (TrackEntry)
	{
		if (strcmp(TrackEntry->animation->name,Die_Action) == 0)
			ActionEnd(Die_Action);
	}else{
		CCLOG("[ *ERROR ] ActObject::SpineEnd spTrackEntry NULL");
	}
}
//在循环回调的时候判断动作结束处理
void ActObject::SpineComplete( int trackIndex,int loopCount )
{
	spTrackEntry*TrackEntry = ((SkeletonAnimation*)m_Armature)->getCurrent(trackIndex);
	if (TrackEntry)
	{
		if (m_Skeleton)
		{
			m_Skeleton->setToSetupPose();
			m_Skeleton->setVisible(false);
		}
		ActionEnd(TrackEntry->animation->name);
	}else{
		CCLOG("[ *ERROR ] ActObject::SpineComplete spTrackEntry NULL");
	}
}

void ActObject::attackActionEnd()
{
	TurnStateTo(Stand_Index);
	if (m_Alive->getCritEffect()&&!m_Enemy)		
		NOTIFICATION->postNotification(B_CritEnd,m_Alive);
	if (m_Alive->getHp()<=0)
		this->AliveDie();									//释放完技能后自己死亡的情况处理
	else if (m_Alive->getBaseData()->getCallType() == AutoSkill||
		m_Alive->getBaseData()->getCallType() == OnlyOnce)				//陨石类释放攻击后死亡OnlyOnces
		this->AliveDie();	
	AtkEnd_Event();	
}

void ActObject::ActionEnd( const char* ActionName )
{
	if (strcmp(ActionName,Attack_Action) == 0		||			//普攻转站立
		strcmp(ActionName,SpAttack_Action) == 0		||			//特攻转站立			
		strcmp(ActionName,Skill_Action) == 0		||			//必杀技转站立
		strcmp(ActionName,Win_Action)==0		)				//胜利动作转站立		
	{
		attackActionEnd();												
	}else if ( !strcmp(ActionName,Hit_Action) ||				//受击转站立
			   !strcmp(ActionName,Dizzy_Action)		)			//眩晕转站立
	{
		TurnStateTo(Stand_Index);
	}else if (!strcmp(ActionName,Die_Action))
	{
		dieActionEnd();
	}else if (!strcmp(ActionName,Start_Action))
	{
		TurnStateTo(Stand_Index);
		m_Alive->setAliveStat(COMMONSTATE);
	}else{
		if (strcmp(ActionName,Stand_Action)==0||strcmp(ActionName,Walk_Action)==0)
			return;
		CCLOG("[ Tips ] ActObject::animationEvent Action=%s Not Dispose",ActionName);
	}
}

void ActObject::SpineActionEvent( int trackIndex,spEvent* Event )
{
	switch ((EventType)Event->intValue)
	{
	case EventType::BeginEvevt:
		{
			AtkBegin_Event();			//开始攻击(受击数组受伤)
		}break;
	case EventType::HitEvent:
		{
			HpChange_Event();			//显示受击方血量处理
			AtkBegin_Event();			//开始攻击(受击数组受伤)
		}break;
	case EventType::SharkEvent:
		{
			NOTIFICATION->postNotification(B_Shark,nullptr);
		}break;
	case EventType::Mask:
		{
			NOTIFICATION->postNotification(B_SKILL_MASK,m_Alive);
		}break;
	default:
		break;
	}									//动画确定播放点,策划确定播放内容
	if (Event->intValue >= 200)			//音效从200号文件开始播放
		PlaySound_Event(Event->intValue);
}

void ActObject::actionEvent( int pFremIndex )
{
	for(unsigned int frameIndex = m_lastFrame+1; frameIndex<=pFremIndex; frameIndex++)	//防止偶尔的掉帧情况出现处理
	{
		vector<ArmatureEvent*>& frameEvents = m_armatureEventData->getEventVector(m_ActionKey.c_str(), frameIndex);	//获取当前帧的所有事件
		for(unsigned int i=0; i<frameEvents.size(); i++)
		{
			const ArmatureEvent& armatureEvent = *(frameEvents.at(i));
			string sEventName = armatureEvent.name;
			if(sEventName.size()>0)
			{
				sEventName = strRemoveSpace(sEventName);											//去空格
				if(sEventName.compare(EVE_ATKBEGIN)			==0	)		AtkBegin_Event();			//开始攻击(受击数组受伤)
				if(sEventName.compare(EVE_LOSTHP)			==0	)		HpChange_Event();			//显示受击方血量处理
				if(sEventName.compare(EVE_ANIMAT)			==0 )		PlayAnimat_Event(armatureEvent.extraInfo);			//播放动画
				if(sEventName.compare(EVE_SOUND)			==0 )		PlaySound_Event(armatureEvent.sound);				//播放声音
				if(sEventName.compare(EVE_SHAKE)			==0	)
					NOTIFICATION->postNotification(B_Shark,nullptr);
				if (sEventName.compare(EVE_MASK)			==0 )
					NOTIFICATION->postNotification(B_SKILL_MASK,m_Alive);
			}
		}
	}
	m_lastFrame = pFremIndex;
}

void ActObject::updateFrameEvent( float dt )
{
	if (m_IsSpine)
		return;
	if( !m_armatureEventData					||
		m_ActionKey.compare(Stand_Action)==0	||
		m_ActionKey.compare(Hit_Action)==0		||
		m_ActionKey.compare(Dizzy_Action)==0)
		return ;
	int iCurrentFrameIndex = ((CCArmature*)m_Armature)->getAnimation()->getCurrentFrameIndex();
	if(m_lastFrame ==iCurrentFrameIndex)															//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
		return;
	actionEvent(iCurrentFrameIndex);
}

void ActObject::dieActionEnd()
{
	if (m_Alive->getCritEffect()&&!m_Enemy)					//放技能的过程中把自己炸死的特殊情况
		NOTIFICATION->postNotification(B_CritEnd,m_Alive);
	CCFadeOut* fadeout = CCFadeOut::create(0.5f);
	m_Armature->runAction(fadeout);
	getHp()->setVisible(false);
	CCDelayTime* dtime = CCDelayTime::create(0.6f);
	CCCallFuncN* callfunc = CCCallFuncN::create(this ,callfuncN_selector(ActObject::removeAct));
	CCSequence* sqe = CCSequence::create(dtime,callfunc,NULL); 
	this->runAction(sqe);
}

void ActObject::removeAct( CCNode* node )
{
	ActObject* act = (ActObject*) node;
	WarAlive* alive = act->getAlive();
	NOTIFICATION->postNotification(B_ActObjectRemove,alive);		//我方主帅死亡,战斗失败
	if (m_Reset)
	{
		m_Reset = false;		
	}else{
		m_Alive->setActObject(nullptr);
	}
	act->removeFromParentAndCleanup(true); 
	if (CCDirector::sharedDirector()->getScheduler()->getTimeScale() < 1)
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(1);
}

void ActObject::updatePosition(float dt)
{
	int ActionCode = this->getCurrActionCode();
	do{
		if (ActionCode != Walk_Index&&ActionCode!=Hit_Index)
			break;
		int grid = m_MapData->getGridIndex(this->getPosition() - this->getoffs());
		if (grid == m_Alive->getGridIndex()||grid == INVALID_GRID)break;
		m_Alive->setGridIndex(grid);
		break;
	} while (true);

	if (this->getMoveState())
		MoveUpdate(dt);
}
//改进方向，使用像素计算速度，而不是使用格子来计算速度。格子只能用于计算距离
bool ActObject::isDistination(float pDt)
{
	CCPoint pPosition = m_MapData->getPoint(m_Alive->getMoveGrid());
	CCPoint uPosition = this->getPosition() - this->getoffs();

	float pLength = 0;
	float dtLenght = 0;
	if (this->getSpeed().x)
	{
		pLength = fabs(pPosition.x-uPosition.x);									//有一边超过就表示超过了
		dtLenght = fabs(ccpMult(this->getSpeed(),pDt).x);							//每次移动的大小
	}else{
		pLength = fabs(pPosition.y-uPosition.y);									//有一边超过就表示超过了
		dtLenght = fabs(ccpMult(this->getSpeed(),pDt).y);							//每次移动的大小
	}
	if (pLength<=dtLenght)
		return true;
	return false;
}

void ActObject::moveEnd()
{
	CCPoint tPosition = m_MapData->getPoint(m_Alive->getMoveGrid());
	this->setPosition(tPosition+this->getoffs());
	if (!m_Enemy&&m_Alive->getCallType()!=AutoMoveType)
		this->TurnStateTo(Stand_Index);											//站立时会自动将武将方向调转回来
	this->setSpeed(CCPointZero);
	this->setMoveState(0);
}

void ActObject::MoveUpdate( float dt )
{
	if (m_Alive->getMoveGrid() == INVALID_GRID)
	{
		this->setMoveState(0);
		return;
	}
	if (isDistination(dt))
	{
		moveEnd();
	}else{
		this->setPosition(this->getPosition()+ccpMult(this->getSpeed(),dt));
	}
}

bool ActObject::firstBattle( CCPoint& p )
{
	if (m_Alive->getBattle() || m_Alive->getEnemy())								//第一次召唤武将处理(未上阵的武将都瞬移)
		return false;
	m_MoveState = 0;
	m_Alive->setAliveStat(INVINCIBLE);
	this->TurnStateTo(Start_Index);
	NOTIFICATION->postNotification(B_RolrLogInBattlefield,m_Alive);//武将上阵
	m_Alive->setGridIndex(m_Alive->getMoveGrid());
	this->setSpeed(CCPointZero);
	this->setPosition(p);
	return true;
}

void ActObject::setMoveState(int var /*= Walk_Index*/)
{
	m_MoveState = var;
	if (!m_MoveState)
		return;	
	if (!m_Alive->getMove())														//定身状态
	{
		this->TurnStateTo(Stand_Index);
		m_MoveState = 0;
		return;
	}
	if (this->getCurrActionCode() != var)
		this->TurnStateTo(m_MoveState);
	roleMoveSpeed();
}

float ActObject::getMoveTime()
{
	int row = abs((m_Alive->getMoveGrid() % C_GRID_ROW) - (m_Alive->getGridIndex() % C_GRID_ROW));
	int col = abs((m_Alive->getMoveGrid() / C_GRID_ROW) - (m_Alive->getGridIndex() / C_GRID_ROW));
	float tMovetime = max(row,col) * m_Alive->getMoveSpeed();						//一格多少时间
	if (m_MoveState == Hit_Index||!tMovetime)	//被击退的情况
	{
		tMovetime = 0.2f;	
		if (m_MoveState == Hit_Index&&this->getCurrActionCode() != Hit_Index)
			m_Alive->setGridIndex(m_Alive->getMoveGrid());							//动作没有切换成功，瞬间更新受击武将位置(攻击状态无法切换受击动作,无法更新武将位置了)
	}
	return tMovetime;
}

void ActObject::roleMoveSpeed()
{
	int MoveGrid = m_Alive->getMoveGrid();											//格子相等的时候可能存在格子大小的误差，应以点的位置来进行判断
	if (MoveGrid == INVALID_GRID)return;
	CCPoint p = m_MapData->getPoint(MoveGrid) + getoffs();							//目标点
	if (firstBattle(p))
		return ;
	if (m_Alive->getBaseData()->getRoleRow()==1&&m_Alive->getBaseData()->getRoleCol()==1&&m_Enemy)
		p = CCPoint(p.x+CCRANDOM_0_1()*15+5,p.y);									//防止武将叠在一起
	walkDirection(p);
	this->setSpeed((p - getPosition())/getMoveTime());
}

void ActObject::walkDirection( CCPoint& p )
{
	if (this->getCurrActionCode() != Walk_Index)		
		return ;	
	if ( (m_Enemy&&getPosition().x > p.x)  || 
		 (!m_Enemy&&getPosition().x < p.x) )				//移动翻转问题
	{
		if (m_Enemy)									//改变武将方向
		{
			this->setRoleDirection(Ditection_Left);
		}else{
			this->setRoleDirection(Ditection_Right);
		}
	}else{
		if (m_Enemy)									//改变武将方向
		{
			this->setRoleDirection(Ditection_Right);
		}else{
			this->setRoleDirection(Ditection_Left);
		}
	}
}

void ActObject::initMoveObject( CCNode* pMoveParent )
{
	if (m_Alive->getEnemy()	||
		m_Alive->getCaptain()||
		m_Alive->getCallType() != CommonType)
		return;
	MoveObject* tMoveObj = MoveObject::create();
	tMoveObj->setRowCol(m_Alive->getBaseData()->getRoleRow(),m_Alive->getBaseData()->getRoleCol());
	tMoveObj->setMoveAlive(m_Alive);
	tMoveObj->setOffs(getoffs().x,getoffs().y);
	tMoveObj->initMoveSprite();
	tMoveObj->setgrid(m_Alive->getGridIndex());
	m_Alive->setMoveGrid(m_Alive->getGridIndex());
	m_Alive->setMoveObject(tMoveObj);
	setMoveObject(tMoveObj);
	pMoveParent->addChild(tMoveObj);
}

void ActObject::setActMoveGrid( int pGird )
{
	getMoveObject()->setgrid(pGird);
	m_Alive->setMoveGrid(pGird);
	setMoveState(Walk_Index);
}
