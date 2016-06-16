#ifndef _GAME_TRANSITION_H_
#define _GAME_TRANSITION_H_
#include "StateManager.h"
namespace BattleSpace{
	class GameStateTransition : public Transition
	{
	public:
		GameStateTransition();
		virtual ~GameStateTransition();
		CREATE_FUNC(GameStateTransition);
		virtual bool init(){return true;};
		void RoleTransition(bool isEnemy = true);
	public:	
	};
};
#endif // !_GAME_TRANSITION_H_
