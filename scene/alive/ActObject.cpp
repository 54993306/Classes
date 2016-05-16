
#include "GMessage.h"
#include "ActObject.h"
#include "tools/StringUtil.h"
#include "model/WarManager.h"
#include "model/DataCenter.h"
#include "model/BuffManage.h"
#include "model/DataDefine.h"
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
	return true;
}

void ActObject::setActionKey(string ActionKey)//设置人物对应动作(动作切换)然后刷新人物运行动作
{
	if( ActionKey.c_str() == nullptr || !strcmp(ActionKey.c_str(),"") || !m_Armature)
		return;

	if (ActionKey == Walk_Action)			//我方突然中断整个战斗流程的情况(玩家操纵拖动武将)
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
	if (DataCenter::sharedData()->getWar()->isSpine(m_Model))
	{
		SkeletonAnimation* Skeleton = (SkeletonAnimation*)m_Armature;
		for (int i = 0;i<Skeleton->state->data->skeletonData->animationsCount;i++)
		{
			if (strcmp(Skeleton->state->data->skeletonData->animations[i]->name,ActionKey.c_str())==0)
			{
				if (strcmp(ActionKey.c_str(),Die_Action) == 0)
					((SkeletonAnimation*)m_Armature)->setAnimation(0,ActionKey.c_str(),false);
				else
					((SkeletonAnimation*)m_Armature)->setAnimation(0,ActionKey.c_str(),true);
				m_ActionKey = ActionKey;
				if (m_Skeleton)
				{
					for (int i = 0;i<m_Skeleton->skeleton->data->animationsCount;i++)
					{
						if (strcmp(m_Skeleton->skeleton->data->animations[i]->name,ActionKey.c_str())==0)
						{
							if (ActionKey.compare(Stand_Action)==0)
							{
								m_Skeleton->setVisible(false);
								return;
							}
							m_Skeleton->setVisible(true);
							m_Skeleton->setAnimation(0,ActionKey.c_str(),false);
							return;
						}
					}
					m_Skeleton->setToSetupPose();
					m_Skeleton->setVisible(false);
				}
				return;
			}
			if (m_Skeleton)
			{
				m_Skeleton->setToSetupPose();
				m_Skeleton->setVisible(false);
			}
		}
		if (strcmp(ActionKey.c_str(),Stand_Action)==0)
		{
			string Action = Skeleton->state->data->skeletonData->animations[0]->name;
			((SkeletonAnimation*)m_Armature)->setAnimation(0,Action.c_str(),true);
			CCLOG("[ *ERROR ] ActObject::setActionKey Lost %s",Stand_Action);
		}
		if (ActionKey.compare(Start_Action)==0)
			m_Alive->setAliveStat(COMMONSTATE);
		else
			CCLOG("[ *ERROR ] ActObject::setActionKey Lost Action=[%s],model=[%d]",ActionKey.c_str(),m_Model);
		this->TurnStateTo(Stand_Index);
	}else{
		CCMovementData* MovementData = ((CCArmature*)m_Armature)->getAnimation()->getAnimationData()->getMovement(ActionKey.c_str());
		if (MovementData)				//判断动作是否存在
		{
			((CCArmature*)m_Armature)->getAnimation()->play(ActionKey.c_str(),0.01f);
		}else{
			if (ActionKey.compare(Start_Action)==0)
				m_Alive->setAliveStat(COMMONSTATE);
			else
				CCLOG("[ *ERROR ] ActObject::setActionKey Lost Action=[%s],model=[%d]",ActionKey.c_str(),m_Model);
			this->TurnStateTo(Stand_Index);
		}
	}
	m_ActionKey = ActionKey;
}

void ActObject::setModel(int model)//设置人物对应模型
{
	if( m_Model == model || !model) 
		return;
	m_Model = model;
	m_Direction = 0;
	if (m_Armature)
		m_Armature->removeFromParentAndCleanup(true);
	WarManager* manage = DataCenter::sharedData()->getWar();
	if ( manage->isSpine(model) )
	{
		SpData* data = manage->getSpineData(ToString(model));
		if (!data)
		{
			data = manage->getSpineData("146");
			CCLOG("[ *ERROR ]  ActObject::setModel Spine Model=%d IS NULL",model); 
		}
		SkeletonAnimation*  Animation = SkeletonAnimation::createWithData(data->first);
		CCAssert(Animation,"ActObject::setModel Spine NULL");
		Animation->endListener = std::bind(&ActObject::SpineActionEnd,this,std::placeholders::_1);
		Animation->completeListener = std::bind(&ActObject::SpineComplete,this,std::placeholders::_1,std::placeholders::_2);
		Animation->eventListener = std::bind(&ActObject::SpineActionEvent,this,std::placeholders::_1,std::placeholders::_2);
		if (m_Enemy)setDirection(Ditection_Left);
		m_Armature = Animation;
		m_IsSpine = true;

		char efName[60] = {0};
		sprintf(efName,"%d_texiao",model);
		SpData* EFdata = manage->getSpineData(efName);
		if (!EFdata)
		{
			CCLOG("[ *TIPS ]  ActObject::setModel Spine Model=%d IS NULL",model); 
		}else{
			m_Skeleton = SkeletonAnimation::createWithData(EFdata->first);
			m_Skeleton->setPosition(ccp(0,-GRID_HEIGHT/2));
			m_Body->addChild(m_Skeleton,1);
		}
	}else{
		m_IsSpine = false;
		CCAnimationData *animationData = CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(ToString(m_Model));
		if (!animationData)													//模型容错性处理
		{
			CCLOG("[ *ERROR ]  ActObject::setModel Animation Model=%d IS NULL",model); 
			m_Model = 516;
		}	
		m_Armature = CCArmature::create(ToString(m_Model));
		CCAssert(m_Armature,"ActObject::setModel AnimationData NULL");
		((CCArmature*)m_Armature)->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(ActObject::AnimationEvent));		//动作结束回调
		//绑定帧事件数据
		m_armatureEventData = DataCenter::sharedData()->getWar()->getArmatureDataMgr()->getArmatureEventData(m_Model);
	}
	m_Armature->setPosition(ccp(0,-GRID_HEIGHT/2));
	m_Body->addChild(m_Armature);
	this->getStateManage()->initState(this);							//设置人物的初始动作为站立也可为其他						
}
//动作结束回调
void ActObject::AnimationEvent(CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	std::string ActionName = movementID;
	ActionName = strRemoveSpace(ActionName);										//去空格
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

void ActObject::TurnToStand( float dt ){ TurnStateTo(Stand_Index); }

void ActObject::ActionEnd( const char* ActionName )
{
	if (	strcmp(ActionName,Attack_Action) == 0					//普攻转站立
		||	strcmp(ActionName,SpAttack_Action) == 0				//特攻转站立
		||	strcmp(ActionName,Hit_Action) == 0						//受击转站立(连续受击问题)
		||	strcmp(ActionName,Dizzy_Action) == 0					//眩晕转站立
		||	strcmp(ActionName,Skill_Action) == 0					//必杀技转站立
		||	strcmp(ActionName,Win_Action)==0		)				//胜利动作转站立		
	{
		TurnStateTo(Stand_Index);
		if (m_Alive->getCritEffect()&&!m_Enemy)		
			NOTIFICATION->postNotification(SKILLEND,m_Alive);
		if (	strcmp(ActionName,Attack_Action) == 0					//普攻转站立
			||	strcmp(ActionName,SpAttack_Action) == 0				//特攻转站立
			||	strcmp(ActionName,Skill_Action) == 0					//必杀技转站立
			||	strcmp(ActionName,Win_Action)==0)
		{
			if (m_Alive->getHp()<=0)
				this->AliveDie();								//释放完技能后自己死亡的情况处理
			else if (m_Alive->role->CallType == AutoSkill||
				m_Alive->role->CallType == OnlyOnce)		//陨石类释放攻击后死亡OnlyOnces
				this->AliveDie();	
			AtkEnd_Event();		
		}														//攻击结束事件处理		
	}else if (strcmp(ActionName,Die_Action) == 0)
	{
		if (m_Alive->getCritEffect()&&!m_Enemy)					//放技能的过程中把自己炸死的特殊情况
			NOTIFICATION->postNotification(SKILLEND,m_Alive);
		CCFadeOut* fadeout = CCFadeOut::create(0.5f);
		m_Armature->runAction(fadeout);
		getHp()->setVisible(false);
		CCDelayTime* dtime = CCDelayTime::create(0.6f);
		CCCallFuncN* callfunc = CCCallFuncN::create(this ,callfuncN_selector(ActObject::removeAct));
		CCSequence* sqe = CCSequence::create(dtime,callfunc,NULL); 
		this->runAction(sqe);
	}else if (strcmp(ActionName,Start_Action)==0)
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

void ActObject::updateFrameEvent( float dt )
{
	if (DataCenter::sharedData()->getWar()->isSpine(m_Model))
		return;
	if( !m_armatureEventData					||
		m_ActionKey.compare(Stand_Action)==0	||
		m_ActionKey.compare(Hit_Action)==0		||
		m_ActionKey.compare(Dizzy_Action)==0)
		return ;
	int iCurrentFrameIndex = ((CCArmature*)m_Armature)->getAnimation()->getCurrentFrameIndex();
	if(m_lastFrame ==iCurrentFrameIndex)															//骨骼帧率与游戏帧率不同例如：游戏2帧骨骼才跑了1帧的情况(高效)
		return;
	for(unsigned int frameIndex = m_lastFrame+1; frameIndex<=iCurrentFrameIndex; frameIndex++)	//防止偶尔的掉帧情况出现处理
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
	m_lastFrame = iCurrentFrameIndex;
}

void ActObject::removeAct( CCNode* node )
{
	ActObject* act = (ActObject*) node;
	WarAlive* alive = act->getAlive();
	NOTIFICATION->postNotification(ACTREMOVE,alive);		//我方主帅死亡,战斗失败
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

void ActObject::setAlive( WarAlive* var )
{
	if (var)
		m_Alive = var;
	else
		CCLOG("[ **ERROR ]  ActObject::setAlive");
	GameStateManager* stateManage = GameStateManager::create();						//继承实现状态管理器
	GameStateTransition* transition = GameStateTransition::create();				//继承实现状态切换列表对象
	this->setStateManager(stateManage);												//设置状态机的状态管理器
	this->setTransition(transition);												//设置状态切换对象
	transition->RoleTransition(m_Alive->getEnemy());
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

void ActObject::MoveUpdate( float dt )
{
	if (m_Alive->getMoveGrid() == INVALID_GRID)
	{
		this->setMoveState(0);
		return;
	}
	CCPoint pPosition = m_MapData->getPoint(m_Alive->getMoveGrid());
	CCPoint uPosition = this->getPosition() - this->getoffs();

	float pLength = 0;
	float dtLenght = 0;
	if (this->getSpeed().x)
	{
		pLength = fabs(pPosition.x-uPosition.x);									//有一边超过就表示超过了
		dtLenght = fabs(ccpMult(this->getSpeed(),dt).x);							//每次移动的大小
	}else{
		pLength = fabs(pPosition.y-uPosition.y);									//有一边超过就表示超过了
		dtLenght = fabs(ccpMult(this->getSpeed(),dt).y);							//每次移动的大小
	}
	if (pLength<=dtLenght)
	{
		this->setPosition(pPosition+this->getoffs());
		if (!m_Enemy&&m_Alive->getCallType()!=AutoMoveType)
			this->TurnStateTo(Stand_Index);											//站立时会自动将武将方向调转回来
		this->setSpeed(CCPointZero);
		this->setMoveState(0);
		return;
	}else{
		this->setPosition(this->getPosition()+ccpMult(this->getSpeed(),dt));
	}
}

bool ActObject::firstBattle( CCPoint& p )
{
	if (!m_Alive->getBattle()&&!m_Alive->getEnemy())								//第一次召唤武将处理(未上阵的武将都瞬移)
	{
		m_MoveState = 0;
		m_Alive->setAliveStat(INVINCIBLE);
		this->TurnStateTo(Start_Index);
		NOTIFICATION->postNotification(B_RolrLogInBattlefield,m_Alive);//武将上阵
		m_Alive->setGridIndex(m_Alive->getMoveGrid());
		this->setSpeed(CCPointZero);
		this->setPosition(p);
		return true;
	}
	return false;
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

void ActObject::roleMoveSpeed()
{
	int MoveGrid = m_Alive->getMoveGrid();											//格子相等的时候可能存在格子大小的误差，应以点的位置来进行判断
	if (MoveGrid == INVALID_GRID)return;
	CCPoint p = m_MapData->getPoint(MoveGrid) + getoffs();							//目标点
	if (firstBattle(p))
		return ;
	if (m_Alive->role->row==1&&m_Alive->role->col==1&&m_Enemy)
		p = CCPoint(p.x+CCRANDOM_0_1()*15+5,p.y);									//防止武将叠在一起
	int us_grid = m_Alive->getGridIndex();
	int row = abs((MoveGrid % C_GRID_ROW) - (us_grid % C_GRID_ROW));
	int col = abs((MoveGrid / C_GRID_ROW) - (us_grid / C_GRID_ROW));
	float actionTime = max(row,col) * m_Alive->getMoveSpeed();						//一格多少时间
	if (m_MoveState == Hit_Index||!actionTime)	//被击退的情况
	{
		actionTime = 0.2f;	
		if (m_MoveState == Hit_Index&&this->getCurrActionCode() != Hit_Index)
			m_Alive->setGridIndex(MoveGrid);										//动作没有切换成功，瞬间更新受击武将位置(攻击状态无法切换受击动作,无法更新武将位置了)
	}
	CCPoint cp = getPosition();														//武将当前坐标
	this->setSpeed((p - cp)/actionTime);
	walkDirection(p,cp);
}

void ActObject::walkDirection( CCPoint& p,CCPoint& cp )
{
	if (this->getCurrActionCode() != Walk_Index)		
		return ;	
	if ((m_Enemy&&cp.x > p.x) || (!m_Enemy&&cp.x < p.x))//移动翻转问题
	{
		if (m_Enemy)									//改变武将方向
		{
			this->setDirection(Ditection_Left);
		}else{
			this->setDirection(Ditection_Right);
		}
	}else{
		if (m_Enemy)									//改变武将方向
		{
			this->setDirection(Ditection_Right);
		}else{
			this->setDirection(Ditection_Left);
		}
	}
}
MoveObj::MoveObj():m_grid(0),m_Actobject(nullptr),m_Alive(nullptr){}

bool MoveObj::init()
{
	if (CCNode::init())
		return true;
	return false;
}

void MoveObj::setActObject(ActObject* var)
{
	m_Actobject = var;
	CCSprite* ef = nullptr;

	for (int i=0;i<m_Alive->role->row;i++)
		for (int j =0;j<m_Alive->role->col;j++)
		{
			ef = CCSprite::create("warScene/fanglandingwei.png");
			ef->setPosition(ccp(-m_Actobject->getoffs().x+(j*(GRID_WIDTH+C_GRIDOFFSET_X)),-m_Actobject->getoffs().y-(i*(GRID_HEIGHT+C_GRIDOFFSET_Y))));
			addChild(ef);
		}
}

ActObject* MoveObj::getActObject(){return m_Actobject;}

void MoveObj::setAlive(WarAlive* var)
{
	if (!m_Alive)
	{
		m_Alive = var;
	}else{CCLOG(" [ TIPS :MoveObj::setAlive SetAlive Repeat]");}
	if (m_Alive->getCallType() != CommonType)
		this->setVisible(false);
}

WarAlive* MoveObj::getAlive(){return m_Alive;}

void MoveObj::setgrid(int var)
{
	if (var == m_grid)
		return;
	grids.clear();
	for (int i=0;i<m_Alive->role->row;i++)
		for (int j =0;j<m_Alive->role->col;j++)
			grids.push_back(var+j*C_GRID_ROW+i);
	sort(grids.begin(),grids.end());
	m_grid = grids.at(0);
	WarMapData* Warmap = DataCenter::sharedData()->getMap()->getCurrWarMap();
	CCPoint p = Warmap->getPoint(m_grid);
	m_Actobject->getAlive()->setMoveGrid(m_grid);
	setPosition(p+m_Actobject->getoffs());	
}
int MoveObj::getgrid(){return m_grid;}