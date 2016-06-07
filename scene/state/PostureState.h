#ifndef _POSTURE_STATE_H_
#define _POSTURE_STATE_H_
#include "StateManager.h"
/************************************************************************/
/*           
					//对武将的所有状态进行一个声明和动作间切换	
					//  [11/12/2014 xc_lin]
*/
/************************************************************************/
namespace BattleSpace{
	enum RoleStateDefine
	{
		Stand_Index = 1,		//1、 站立
		Walk_Index,				//2、 行走
		Hit_Index,				//3、 受击
		Dizzy_Index,			//4、 眩晕
		Attack_Index,			//5、 攻击
		SpAttack_Index,			//6、 特殊攻击
		Skill_Index,			//7、 主动技能
		victory_Index,			//8、 胜利动作
		Die_Index,				//9、 死亡
		Start_Index,			//10、入场
	};

	class PostureState :public State
	{
	public:
		PostureState(){};
		virtual ~PostureState(){};
		virtual bool ExecuteNextState(StateManager* actor,StateMachine* self);
		virtual bool canTransition(StateMachine* self,State* targetState);//重载父类方法，根据状态机判断当前状态是否可以切换到目标状态
	};
	//1站立
	class StandState:public PostureState
	{
	public:
		StandState(){m_stateCode = Stand_Index;};
		virtual ~StandState(){};
		CREATE_FUNC(StandState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//2走路
	class WalkState:public PostureState
	{
	public:
		WalkState(){m_stateCode = Walk_Index;};
		virtual ~WalkState(){};
		CREATE_FUNC(WalkState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//3受击
	class HitState:public PostureState
	{
	public:
		HitState(){m_stateCode = Hit_Index;};
		virtual ~HitState(){};
		CREATE_FUNC(HitState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//4晕眩
	class DizzyState:public PostureState
	{
	public:
		DizzyState(){m_stateCode = Dizzy_Index;};
		virtual ~DizzyState(){};
		CREATE_FUNC(DizzyState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//5攻击
	class AttackState:public PostureState
	{
	public:
		AttackState(){m_stateCode = Attack_Index;};
		virtual ~AttackState(){};
		CREATE_FUNC(AttackState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//6特殊攻击
	class SpecialAttackState:public PostureState
	{
	public:
		SpecialAttackState(){m_stateCode = SpAttack_Index;};
		virtual ~SpecialAttackState(){};
		CREATE_FUNC(SpecialAttackState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//7主动技能
	class SkillState:public PostureState
	{
	public:
		SkillState(){m_stateCode = Skill_Index;};
		virtual ~SkillState(){};
		CREATE_FUNC(SkillState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//8胜利动作
	class VictoryState:public PostureState
	{
	public:
		VictoryState(){m_stateCode = victory_Index;};
		virtual ~VictoryState(){};
		CREATE_FUNC(VictoryState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//9死亡
	class DiedState:public PostureState
	{
	public:
		DiedState(){m_stateCode = Die_Index;};
		virtual ~DiedState(){};
		CREATE_FUNC(DiedState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
	//10入场
	class StartState:public PostureState
	{
	public:
		StartState(){m_stateCode = Start_Index;};
		virtual ~StartState(){};
		CREATE_FUNC(StartState);
		virtual void entryState(StateManager* actor,StateMachine* self);
		virtual void leaveState(StateManager* actor,StateMachine* self){};
	};
};
#endif // !_POSTURE_STATE_H_