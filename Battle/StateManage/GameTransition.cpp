#include "GameTransition.h"
#include "PostureState.h"
namespace BattleSpace{

	GameStateTransition::GameStateTransition()
	{}

	GameStateTransition::~GameStateTransition()
	{}
	//根据不同的类型可以进行不同类型的武将状态切换处理
	void GameStateTransition::RoleTransition(bool isEnemy/*=true*/)
	{
		//1、站立
		StateVector stand;	
		stand.push_back(E_StateCode::eStandState);
		stand.push_back(E_StateCode::eEnterState);
		stand.push_back(E_StateCode::eWalkState);
		stand.push_back(E_StateCode::eHitState);
		stand.push_back(E_StateCode::eDizzyState);
		stand.push_back(E_StateCode::eNormalAttackState);
		stand.push_back(E_StateCode::eVictoryState);
		stand.push_back(E_StateCode::eSpecialAttackState);
		stand.push_back(E_StateCode::eActiveAttackState);
		stand.push_back(E_StateCode::eDeathState);	
		//2、行走
		StateVector walk;
		walk.push_back(E_StateCode::eStandState);
		walk.push_back(E_StateCode::eEnterState);
		walk.push_back(E_StateCode::eDeathState);
		walk.push_back(E_StateCode::eWalkState);	
		//3、受击
		StateVector hit;
		hit.push_back(E_StateCode::eStandState);
		hit.push_back(E_StateCode::eWalkState);
		hit.push_back(E_StateCode::eDeathState);
		hit.push_back(E_StateCode::eActiveAttackState);
		hit.push_back(E_StateCode::eVictoryState);
		hit.push_back(E_StateCode::eNormalAttackState);
		hit.push_back(E_StateCode::eSpecialAttackState);
		hit.push_back(E_StateCode::eHitState);
		//4、眩晕
		StateVector dizzy;
		dizzy.push_back(E_StateCode::eStandState);
		dizzy.push_back(E_StateCode::eHitState);
		dizzy.push_back(E_StateCode::eDeathState);
		dizzy.push_back(E_StateCode::eNormalAttackState);
		dizzy.push_back(E_StateCode::eVictoryState);
		dizzy.push_back(E_StateCode::eSpecialAttackState);
		dizzy.push_back(E_StateCode::eActiveAttackState);	
		//5、攻击
		StateVector Attck;
		Attck.push_back(E_StateCode::eStandState);
		Attck.push_back(E_StateCode::eDizzyState);
		Attck.push_back(E_StateCode::eDeathState);
		//6、特殊攻击
		StateVector specialAttack;
		specialAttack.push_back(E_StateCode::eStandState);
		specialAttack.push_back(E_StateCode::eDizzyState);
		specialAttack.push_back(E_StateCode::eDeathState);
		//7、主动技能
		StateVector skill;
		skill.push_back(E_StateCode::eStandState);
		skill.push_back(E_StateCode::eDizzyState);
		skill.push_back(E_StateCode::eDeathState);	
		skill.push_back(E_StateCode::eWalkState);
		//8、胜利动作(攻击动作中的一种)
		StateVector victory;
		victory.push_back(E_StateCode::eStandState);
		victory.push_back(E_StateCode::eDizzyState);
		victory.push_back(E_StateCode::eWalkState);	
		victory.push_back(E_StateCode::eDeathState);
		//10、入场
		StateVector start;
		start.push_back(E_StateCode::eStandState);
		start.push_back(E_StateCode::eWalkState);
		if (!isEnemy)
		{
			Attck.push_back(E_StateCode::eWalkState);
			specialAttack.push_back(E_StateCode::eWalkState);
			hit.push_back(E_StateCode::eWalkState);

			Attck.push_back(E_StateCode::eActiveAttackState);
			specialAttack.push_back(E_StateCode::eActiveAttackState);
		}
		m_transitions[E_StateCode::eEnterState] = start;
		m_transitions[E_StateCode::eStandState] = stand;
		m_transitions[E_StateCode::eWalkState] = walk;
		m_transitions[E_StateCode::eHitState] = hit;
		m_transitions[E_StateCode::eDizzyState] = dizzy;
		m_transitions[E_StateCode::eNormalAttackState] = Attck;
		m_transitions[E_StateCode::eSpecialAttackState] = specialAttack;
		m_transitions[E_StateCode::eActiveAttackState] = skill;
		m_transitions[E_StateCode::eVictoryState] = victory;
	}
};