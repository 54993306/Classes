#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_
#include "cocos2d.h"
#include <map>
#include <vector>
using namespace std;
USING_NS_CC;
namespace BattleSpace{
	class StateMachine;
	class State;
	class StateManager;

	enum struct sStateCode
	{
		eNullState						= 0,
		eStandState						= 1,	//1、 站立
		eWalkState						= 2,	//2、 行走
		eHitState						= 3,	//3、 受击
		eDizzyState						= 4,	//4、 眩晕
		eNormalAttackState				= 5,	//5、 攻击
		eSpecialAttackState				= 6,	//6、 特殊攻击
		eActiveAttackState				= 7,	//7、 主动技能
		eVictoryState					= 8,	//8、 胜利动作
		eDeathState						= 9,	//9、 死亡
		eEnterState						= 10,	//10、入场
	};

	typedef map<sStateCode,State*> CodeState;			//状态码和对应状态map
	typedef vector<sStateCode> StateVector;			//当前状态的可过渡状态  例如：行走可过渡站立，可过渡死亡等
	typedef map<sStateCode,StateVector> StateMap;		//状态id，对应的可切换状态列表 例如 例如：1状态为行走，可过渡站立和死亡，2状态为站立，可过渡为行走与受击

	class State :public CCObject//状态对象
	{
	public:
		State():m_stateCode(sStateCode::eNullState){};
		virtual ~State(){};
		inline sStateCode getCode(){return m_stateCode;}									//状态码
		inline void setCode(sStateCode code){m_stateCode = code;}
		virtual bool canTransition(StateMachine* self,State* pState){return false;};//根据状态机判断是否可以切换至目标状态
		virtual void entryState(StateManager* actor,StateMachine* self){};			//进入状态(未实现的主要用于子类重载)
		virtual bool ExecuteNextState(StateManager* manager,StateMachine* self){return false;}	//执行下一个状态(传入一个状态管理器)
		virtual void leaveState(StateManager* actor,StateMachine* self){};			//离开状态
		virtual bool init(){return true;}	
	protected:
		sStateCode m_stateCode;
	};

	class Transition : public CCObject												//状态切换管理对象
	{
	public:
		Transition(){};
		virtual ~Transition();
	public:
		StateVector getCurrentStateTransitionVec(sStateCode stateCode);					//得到状态stateCode的可过渡状态列表
		virtual void excuteAiLogic(StateMachine* user,CCObject* param){};			//执行AI逻辑(执行AI判断当前状态可切换状态)
		inline bool init(){return true;}
	protected:
		StateMap m_transitions;														//每一个状态都有一个切换状态列表
	};

	class StateMachine																//状态机，负责切换状态和状态管理
	{
	public:
		StateMachine();
		virtual ~StateMachine();
	public:
		inline Transition* getTransition(){return m_transition;};								//得到过渡对象
		inline State* getCurrentPostureState(){return m_currentPostureState;};					//得到当前姿态
		void setCurrentPostureState(State* state);												//设置当前姿态
		inline sStateCode getCurrActionCode(){return m_currentPostureState->getCode();}	//得到当前状态代码
		inline void setCurrActionCode(sStateCode code){m_currentPostureState->setCode(code);}
		virtual bool TurnStateTo(sStateCode stateCode);											//切换至目标状态(返回切换是否成功)  
		inline sStateCode getNextActionCode(){return m_nextActionCode;}								//得到下一个动作code
		inline void setNextActionCode(sStateCode stateCode){m_nextActionCode = stateCode;}				//设置下一个动作
		inline StateManager* getStateManage(){return m_stateManager;}							//得到状态管理对象
		inline void setBlocked(bool block){m_blocked = block;}									//设置是否为阻塞的
		inline bool getBlocked(){return m_blocked;}
	protected:
		void setTransition(Transition* transition);												//设置过渡对象
		void setStateManager(StateManager* stateManager);										//设置状态管理
	protected:
		Transition* m_transition;
		State* m_currentPostureState;
		sStateCode m_nextActionCode;
		bool m_blocked;
		StateManager* m_stateManager;
	};

	class StateManager:public CCObject													//状态管理对象，主要负责管理状态的种类
	{
	public:
		StateManager(){};
		virtual ~StateManager();
	public:
		State* getStateInstance(sStateCode stateCode );										//根据状态code得到状态对象
		virtual void initState(StateMachine* self){};									//初始化
		bool isInTransitionVec(StateMachine* self,sStateCode currentState,sStateCode targetState );	//判断当前状态是否可以切换至目标状态
		StateVector getTransitionVec(StateMachine* self,sStateCode stateCode );				//根据状态机得到可过渡状态vector
		bool switchPostureState(StateMachine* self,State* targetState);//当前状态切换至目标状态
		virtual bool init(){return true;}
	protected:
		void registerState(State* state);												//注册状态
	protected:
		CodeState m_MapCodeState;														//每一个状态机都存在一个状态map
	};
};
#endif // !_STATE_MANAGER_H_
