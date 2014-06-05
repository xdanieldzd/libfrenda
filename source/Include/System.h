/*------------------------------------------------------------
	INCLUDE/SYSTEM.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_SYSTEM_H
#define _INCLUDED_SYSTEM_H

namespace frenda
{
	class System
	{
		public:
			System();
			void initializeDebug(std::string);
			void System::debugLog(const char *fmt, ...);
			bool buttonPressed(uint32, uint32, uint32);
			bool buttonHeld(uint32, uint32, uint32);
			bool isDriveOpen();
			void doFrame();
			void shutdown();
			
			bool debugEnabled, osdLogEnabled;
			
			frenda::PVR *pvr;
			frenda::Sound *sound;
			frenda::Font *dbgFont;
			frenda::StateManager *mgr;
			
			uint32 mapleFunctionCodes[MAPLE_PORT_COUNT][MAPLE_UNIT_COUNT];
			cont_state_t ctrlStatus[MAPLE_PORT_COUNT][MAPLE_UNIT_COUNT], lastCtrlStatus[MAPLE_PORT_COUNT][MAPLE_UNIT_COUNT];
			
		private:
			struct MessageStruct
			{
				char text[256];
				float timer;
			};
			
			std::deque<MessageStruct> messages;
			static const int messagesMax;
	};
}

#endif
