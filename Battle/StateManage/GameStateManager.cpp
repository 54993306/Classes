#include "GameStateManager.h"
#include "PostureState.h"
namespace BattleSpace{
	GameStateManager::GameStateManager()
	{}

	GameStateManager::~GameStateManager()
	{}


	void GameStateManager::initState(StateMachine* self)			//初始化每一个武将的状态
	{
		StateManager* stateManage = self->getStateManage();
		State* stand = getStateInstance(E_StateCode::eStandState);				//根据状态id得到状态
		self->setCurrentPostureState(stand);						//设置初始状态
		stand->entryState(stateManage,self);						//进入状态,状态机其实就是显示的对象(武将)
	}

	bool GameStateManager::init()									//在状态管理器中注册状态
	{
		registerState(StandState::create());						//1、 站立
		registerState(WalkState::create());							//2、 行走
		registerState(HitState::create());							//3、 受击
		registerState(DizzyState::create());						//4、 眩晕
		registerState(AttackState::create());						//5、 攻击
		registerState(SpecialAttackState::create());				//6、 特殊攻击
		registerState(SkillState::create());						//7、 主动技能
		registerState(VictoryState::create());						//8、 胜利动作
		registerState(DiedState::create());							//9、 死亡
		registerState(StartState::create());						//10、入场
		return true;
	}
}
