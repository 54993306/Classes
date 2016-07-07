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
		stand.push_back(sStateCode::eStandState);
		stand.push_back(sStateCode::eEnterState);
		stand.push_back(sStateCode::eWalkState);
		stand.push_back(sStateCode::eHitState);
		stand.push_back(sStateCode::eDizzyState);
		stand.push_back(sStateCode::eNormalAttackState);
		stand.push_back(sStateCode::eVictoryState);
		stand.push_back(sStateCode::eSpecialAttackState);
		stand.push_back(sStateCode::eActiveAttackState);
		stand.push_back(sStateCode::eDeathState);	
		//2、行走
		StateVector walk;
		walk.push_back(sStateCode::eStandState);
		walk.push_back(sStateCode::eEnterState);
		walk.push_back(sStateCode::eDeathState);
		walk.push_back(sStateCode::eWalkState);	
		//3、受击
		StateVector hit;
		hit.push_back(sStateCode::eStandState);
		hit.push_back(sStateCode::eWalkState);
		hit.push_back(sStateCode::eDeathState);
		hit.push_back(sStateCode::eActiveAttackState);
		hit.push_back(sStateCode::eVictoryState);
		hit.push_back(sStateCode::eNormalAttackState);
		hit.push_back(sStateCode::eSpecialAttackState);
		hit.push_back(sStateCode::eHitState);
		//4、眩晕
		StateVector dizzy;
		dizzy.push_back(sStateCode::eStandState);
		dizzy.push_back(sStateCode::eHitState);
		dizzy.push_back(sStateCode::eDeathState);
		dizzy.push_back(sStateCode::eNormalAttackState);
		dizzy.push_back(sStateCode::eVictoryState);
		dizzy.push_back(sStateCode::eSpecialAttackState);
		dizzy.push_back(sStateCode::eActiveAttackState);	
		//5、攻击
		StateVector Attck;
		Attck.push_back(sStateCode::eStandState);
		Attck.push_back(sStateCode::eDizzyState);
		Attck.push_back(sStateCode::eDeathState);
		//6、特殊攻击
		StateVector specialAttack;
		specialAttack.push_back(sStateCode::eStandState);
		specialAttack.push_back(sStateCode::eDizzyState);
		specialAttack.push_back(sStateCode::eDeathState);
		//7、主动技能
		StateVector skill;
		skill.push_back(sStateCode::eStandState);
		skill.push_back(sStateCode::eDizzyState);
		skill.push_back(sStateCode::eDeathState);	
		skill.push_back(sStateCode::eWalkState);
		//8、胜利动作(攻击动作中的一种)
		StateVector victory;
		victory.push_back(sStateCode::eStandState);
		victory.push_back(sStateCode::eDizzyState);
		victory.push_back(sStateCode::eWalkState);	
		victory.push_back(sStateCode::eDeathState);
		//10、入场
		StateVector start;
		start.push_back(sStateCode::eStandState);
		if (!isEnemy)
		{
			Attck.push_back(sStateCode::eWalkState);
			specialAttack.push_back(sStateCode::eWalkState);
			hit.push_back(sStateCode::eWalkState);

			Attck.push_back(sStateCode::eActiveAttackState);
			specialAttack.push_back(sStateCode::eActiveAttackState);
		}
		m_transitions[sStateCode::eEnterState] = start;
		m_transitions[sStateCode::eStandState] = stand;
		m_transitions[sStateCode::eWalkState] = walk;
		m_transitions[sStateCode::eHitState] = hit;
		m_transitions[sStateCode::eDizzyState] = dizzy;
		m_transitions[sStateCode::eNormalAttackState] = Attck;
		m_transitions[sStateCode::eSpecialAttackState] = specialAttack;
		m_transitions[sStateCode::eActiveAttackState] = skill;
		m_transitions[sStateCode::eVictoryState] = victory;
	}
};