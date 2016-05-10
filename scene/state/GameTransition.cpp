#include "GameTransition.h"
#include "PostureState.h"

GameStateTransition::GameStateTransition()
{}

GameStateTransition::~GameStateTransition()
{}
//根据不同的类型可以进行不同类型的武将状态切换处理
void GameStateTransition::RoleTransition(bool isEnemy/*=true*/)
{
	//1、站立
	StateVector stand;	
	stand.push_back(Stand_Index);
	stand.push_back(Start_Index);
	stand.push_back(Walk_Index);
	stand.push_back(Hit_Index);
	stand.push_back(Dizzy_Index);
	stand.push_back(Attack_Index);
	stand.push_back(victory_Index);
	stand.push_back(SpAttack_Index);
	stand.push_back(Skill_Index);
	stand.push_back(Die_Index);	
	//2、行走
	StateVector walk;
	walk.push_back(Stand_Index);
	walk.push_back(Start_Index);
	walk.push_back(Die_Index);
	walk.push_back(Walk_Index);	
	//3、受击
	StateVector hit;
	hit.push_back(Stand_Index);
	hit.push_back(Walk_Index);
	hit.push_back(Die_Index);
	hit.push_back(Skill_Index);
	hit.push_back(victory_Index);
	hit.push_back(Attack_Index);
	hit.push_back(SpAttack_Index);
	hit.push_back(Hit_Index);
	//4、眩晕
	StateVector dizzy;
	dizzy.push_back(Stand_Index);
	dizzy.push_back(Hit_Index);
	dizzy.push_back(Die_Index);
	dizzy.push_back(Attack_Index);
	dizzy.push_back(victory_Index);
	dizzy.push_back(SpAttack_Index);
	dizzy.push_back(Skill_Index);	
	//5、攻击
	StateVector Attck;
	Attck.push_back(Stand_Index);
	Attck.push_back(Dizzy_Index);
	Attck.push_back(Die_Index);
	//6、特殊攻击
	StateVector specialAttack;
	specialAttack.push_back(Stand_Index);
	specialAttack.push_back(Dizzy_Index);
	specialAttack.push_back(Die_Index);
	//7、主动技能
	StateVector skill;
	skill.push_back(Stand_Index);
	skill.push_back(Dizzy_Index);
	skill.push_back(Die_Index);	
	skill.push_back(Walk_Index);
	//8、胜利动作(攻击动作中的一种)
	StateVector victory;
	victory.push_back(Stand_Index);
	victory.push_back(Dizzy_Index);
	victory.push_back(Walk_Index);	
	victory.push_back(Die_Index);
	//10、入场
	StateVector start;
	start.push_back(Stand_Index);
	start.push_back(Walk_Index);
	if (!isEnemy)
	{
		Attck.push_back(Walk_Index);
		specialAttack.push_back(Walk_Index);
		hit.push_back(Walk_Index);

		Attck.push_back(Skill_Index);
		specialAttack.push_back(Skill_Index);
	}
	m_transitions[Start_Index] = start;
	m_transitions[Stand_Index] = stand;
	m_transitions[Walk_Index] = walk;
	m_transitions[Hit_Index] = hit;
	m_transitions[Dizzy_Index] = dizzy;
	m_transitions[Attack_Index] = Attck;
	m_transitions[SpAttack_Index] = specialAttack;
	m_transitions[Skill_Index] = skill;
	m_transitions[victory_Index] = victory;
}

void GameStateTransition::registerChange(StateVector& state,int code)
{
	for (int i = 1; i <= 8; i++)
	{
		state.push_back(i);
	}
	m_transitions[code] = state;
}
