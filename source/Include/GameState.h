/*------------------------------------------------------------
	INCLUDE/GAMESTATE.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_GAMESTATE_H
#define _INCLUDED_GAMESTATE_H

namespace frenda
{
	class GameState
	{
		public:
			virtual void initialize() = 0;
			virtual void cleanup() = 0;
			virtual void doFrame() = 0;
			
			bool paused;
			
			void changeState(StateManager *sm, GameState *gs)
			{
				sm->changeState(gs);
			}
			
		protected:
			GameState()
			{
				paused = false;
			}
			
			virtual ~GameState()
			{
				//
			}
	};
}

#endif
