/*------------------------------------------------------------
	STATEMANAGER.CPP - Gamestate manager
 -------------------------------------------------------------*/

#include "include/frenda.h"

namespace frenda
{
	StateManager::StateManager()
	{
		dbglog(DBG_DEBUG, "%s -> initializing state manager...\n", __PRETTY_FUNCTION__);
	}
	
	StateManager::~StateManager()
	{
		for(std::vector<GameState*>::iterator itr = states.begin(); itr != states.end(); ++itr) (*itr)->cleanup();
		states.clear();
	}
	
	void StateManager::changeState(GameState *state) 
	{
		if (!states.empty())
		{
			states.back()->cleanup();
			states.pop_back();
		}
		
		states.push_back(state);
		states.back()->initialize();
	}
	
	void StateManager::pushState(GameState *state)
	{
		if (!states.empty())
		{
			states.back()->paused = true;
		}
		
		states.push_back(state);
		states.back()->initialize();
	}
	
	void StateManager::popState()
	{
		if (!states.empty())
		{
			states.back()->cleanup();
			states.pop_back();
		}
		
		if (!states.empty())
		{
			states.back()->paused = false;
		}
		else
			arch_reboot();
	}
	
	void StateManager::doFrame()
	{
		states.back()->doFrame();
	}
}
