#ifndef _GAME_TRANSITION_H_
#define _GAME_TRANSITION_H_
#include "StateManager.h"
class GameStateTransition : public Transition
{
public:
	GameStateTransition();
	virtual ~GameStateTransition();
	CREATE_FUNC(GameStateTransition);
	virtual bool init(){return true;};
	void RoleTransition(bool isEnemy = true);
	void registerChange(StateVector& state,int code);
public:	
};
#endif // !_GAME_TRANSITION_H_
