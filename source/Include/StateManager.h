/*------------------------------------------------------------
	INCLUDE/STATEMANAGER.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_STATEMANAGER_H
#define _INCLUDED_STATEMANAGER_H

namespace frenda
{
	class GameState;
	
	class StateManager
	{
		public:
			StateManager();
			~StateManager();
			void changeState(GameState *);
			void pushState(GameState *);
			void popState();
			
			void doFrame();
			
		private:
			std::vector<GameState*> states;
	};
}

#endif
