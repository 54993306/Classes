#ifndef _GAME_STATE_MANAGER_H_
#define _GAME_STATE_MANAGER_H_
#include "StateManager.h"
namespace BattleSpace{
	class GameStateManager : public StateManager
	{
	public:
		GameStateManager();
		virtual ~GameStateManager();
		CREATE_FUNC(GameStateManager);
		virtual void initState(StateMachine* self);
		virtual bool init();
	public:

	};
};
#endif // !_GAME_STATE_MANAGER_H_