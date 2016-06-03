
#include "AppUI.h"
#include "Global.h"
#include "PostureState.h"
#include "Battle/BattleRole.h"
#include "warscene/ConstNum.h"
#include "common/CommonFunction.h"
#include "scene/alive/AliveDefine.h"

bool PostureState::ExecuteNextState(StateManager* actor,StateMachine* self)
{
	if (actor&&self)
	{
		int nextActionCode = self->getNextActionCode();					//下一个状态id
		State* nextState = actor->getStateInstance(nextActionCode);		//根据id从状态管理器中拿到状态
		if (nextState)
		{
			return actor->switchPostureState(self,nextState);		//切换至下一个状态
		}else{
			CCLOG("[ *ERROR ] PostureState::ExecuteNextState");
			nextState = actor->getStateInstance(Attack_Index);
			actor->switchPostureState(self,nextState);
			return false;
		}
	}else{
		CCLOG("[ *ERROR ] PostureState::ExecuteNextState");
		return false;
	}
}
//根据状态机判断当前状态是否可以切换到目标状态
bool PostureState::canTransition(StateMachine* self,State* targetState)
{
	bool ret = false;
	do{
		if( self->getBlocked() ) break;												//状态机阻塞则返回
		State* CurrentPostureState = self->getCurrentPostureState();				//得到当前状态
		int CurrentStateCode = CurrentPostureState->getCode();
		int targetStateCode = targetState->getCode();
		StateManager* stateManage = self->getStateManage();							//状态管理器
		if( stateManage->isInTransitionVec(self,CurrentStateCode,targetStateCode))	//判断当前状态是否可以切换至目标状态
		{
			ret = true;
		}
	} while (0);
	return ret;
}
//1站立
void StandState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("StandState::entryState",act))return;
	if (act->getEnemy())										//动作播放完成均做方向判定处理
	{
		act->setRoleDirection(Ditection_Right);
	}else{
		act->setRoleDirection(Ditection_Left);
	}
	act->setActionKey(Stand_Action);
}
//2走路
void WalkState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("WalkState::entryState",act))return;
	act->setActionKey(Walk_Action);
}
//3受击
void HitState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("HitState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(Hit_Action);
}
//4眩晕
void DizzyState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("DizzyState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(Dizzy_Action);
}
//5攻击
void AttackState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("AttackState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(Attack_Action);
}
//6特殊攻击
void SpecialAttackState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("SpecialAttackState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(SpAttack_Action);
}
//7主动技能
void SkillState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("SkillState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(Skill_Action);
}
//8胜利动作
void VictoryState::entryState(StateManager* actor,StateMachine* self)
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("SkillState::entryState",act))return;
	act->setVisible(true);
	act->setActionKey(Win_Action);
}
//9死亡
void DieState::entryState(StateManager* actor,StateMachine* self)
{
	if(outPutERRORMsg("DieState::entryState",self))return;
	ActObject* act = (ActObject*) self;
	act->setActionKey(Die_Action);
	act->setMoveState(0);
	act->setVisible(true);
}
//10入场
void StartState::entryState( StateManager* actor,StateMachine* self )
{
	ActObject* act = (ActObject*) self;
	if(outPutERRORMsg("StandState::entryState",act))return;
	if (act->getEnemy())										//动作播放完成均做方向判定处理
	{
		act->setRoleDirection(Ditection_Right);
	}else{
		act->setRoleDirection(Ditection_Left);
	}
	act->setActionKey(Start_Action);
	if (act->getModel() == 1056)			//海兽出场特殊处理
	{
		act->setPosition(ccpAdd(act->getPosition(),ccp(-200,0)));
		act->runAction(CCSequence::create(CCDelayTime::create(2.0f),CCMoveBy::create(1.0f,ccp(200,0)),NULL));
	}
}