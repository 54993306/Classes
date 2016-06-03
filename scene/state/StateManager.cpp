#include "StateManager.h"
#include "PostureState.h"
#include "common/CommonFunction.h"

StateManager::~StateManager()
{
	for(CodeState::iterator iter = m_MapCodeState.begin(); iter != m_MapCodeState.end();++iter)
		iter->second->release();
	m_MapCodeState.clear();
}
State* StateManager::getStateInstance(int stateCode )//根据code返回状态对象
{
	do{
		CodeState::iterator iter = m_MapCodeState.find(stateCode);
		if( iter != m_MapCodeState.end() )
			return iter->second;
	} while (0);
	return nullptr;
}
bool StateManager::isInTransitionVec(StateMachine* self,int currentState,int targetState )
{
	//当前状态传入状态机中得到可切换的状态list判断目标状态是否存在list中
	StateVector list = getTransitionVec(self,currentState);
	do{
		for(StateVector::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			if(*iter == targetState)
				return true;
		}
	} while (0);
	return false;
}
//得到当前状态下可切换的状态vector
StateVector StateManager::getTransitionVec(StateMachine* self,int stateCode )
{
	return self->getTransition()->getCurrentStateTransitionVec(stateCode);		
}
//如果当前状态可以切换至设置状态则切换为设置状态
bool StateManager::switchPostureState(StateMachine* self,State* targetState)
{
	if( self->getCurrentPostureState()->canTransition( self , targetState ) )
	{
		self->getCurrentPostureState()->leaveState( this , self );	//脱离状态
		self->setCurrentPostureState(targetState);					//设置状态
		self->getCurrentPostureState()->entryState( this , self );	//进入状态
		return true;
	}else{
		if (targetState->getCode() != Hit_Index)
			CCLOG("[ *TIPS ]StateManager::switchPostureState State Change Fail [ %d ] ---> [ %d ]",self->getCurrentPostureState()->getCode(),targetState->getCode());
		return false;												
	}
}
//在状态管理器中注册状态,不同对象可存在不同数量状态
void StateManager::registerState(State* state)
{
	if(state == nullptr) return;
	state->retain();
	m_MapCodeState[state->getCode()] = state;
}
Transition::~Transition()
{
	for(StateMap::iterator iter = m_transitions.begin(); iter != m_transitions.end(); ++iter)
		iter->second.clear();
	m_transitions.clear();
}
StateVector Transition::getCurrentStateTransitionVec(int stateCode)
{
	StateVector transiton;
	do{
		StateMap::iterator iter = m_transitions.find(stateCode);
		if(iter != m_transitions.end())
			transiton = iter->second;
	} while (0);
	return transiton;
}
StateMachine::StateMachine()
	:m_blocked(false)
	,m_currentPostureState(nullptr)
	,m_nextActionCode(-1)
	,m_stateManager(nullptr)
	,m_transition(nullptr)
{

}
StateMachine::~StateMachine()
{
	CC_SAFE_RELEASE(m_currentPostureState);
	CC_SAFE_RELEASE(m_stateManager);
	CC_SAFE_RELEASE(m_transition);
	m_currentPostureState = nullptr;
	m_stateManager = nullptr;
	m_transition = nullptr;
}
void StateMachine::setCurrentPostureState(State* state)
{
	if(m_currentPostureState == state)
		return;
	CC_SAFE_RELEASE(m_currentPostureState);
	state->retain();
	m_currentPostureState = state;
}
void StateMachine::setTransition(Transition* transition)
{
	if(m_transition == transition)
		return;
	CC_SAFE_RELEASE(m_transition);
	transition->retain();
	m_transition = transition;
}
void StateMachine::setStateManager(StateManager* stateManager)
{
	if(m_stateManager == stateManager)
		return;
	CC_SAFE_RELEASE(m_stateManager);
	stateManager->retain();
	m_stateManager = stateManager;
}
//返回切换是否成功
bool StateMachine::TurnStateTo(int stateCode)
{
	m_nextActionCode = stateCode;
	return m_currentPostureState->ExecuteNextState(m_stateManager,this);			//可以由其他状态管理对象控制切换规则
}