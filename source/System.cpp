/*------------------------------------------------------------
	SYSTEM.CPP - Basic system functions
 -------------------------------------------------------------*/

#include "include/frenda.h"

namespace frenda
{
	const int System::messagesMax = 8;
	
	System::System()
	{
		/* Install custom assert handler */
		assert_set_handler(assertHandler);
		
		/* Let the world (with debuggers) know about Frenda! ^^ */
		debugLog("---------------------------------------------------------------------------\n"
				"%s (%s %s; %s)\n"
				"---------------------------------------------------------------------------\n",
				FRENDATITLE, FRENDA_BUILD_DATE, FRENDA_BUILD_TIME, FRENDA_BUILD_CREATOR);
		
		debugLog("-- Initializing System --\n");
		
		/* Init PVR, Parallax lib, sound, state manager & respective handlers */
		pvr = new frenda::PVR();
		plx_cxt_init();
		sound = new frenda::Sound();
		mgr = new frenda::StateManager();
		
		/* Init debug font to NULL & debug toggle to false*/
		dbgFont = NULL;
		debugEnabled = osdLogEnabled = false;
		
		/* Done! */
		debugLog("-- System Initialized! --\n");
	}
	
	void System::initializeDebug(std::string fontname)
	{
		/* Load debug font */
		dbgFont = new frenda::Font(this, fontname);
		
		/* Init debugging */
		debugEnabled = true;
		messages = std::deque<MessageStruct>(messagesMax);
	}
	
	void System::debugLog(const char *fmt, ...)
	{
		char buf[256];
		va_list vl;
		va_start(vl, fmt);
		memset(buf, 0, sizeof(buf));
		vsnprintf(buf, sizeof(buf), fmt, vl);
		va_end(vl);
		
		/* Print to normal debug log */
		dbglog(DBG_DEBUG, buf);
		
		/* If debugging & OSD log are enabled specifically, print to screen */
		if(debugEnabled && osdLogEnabled && dbgFont != NULL)
		{
			if((signed)messages.size() >= messagesMax)
			{
				messages.pop_back();
			}
			
			MessageStruct msg;
			memcpy(msg.text, buf, sizeof(buf));
			msg.timer = 4.0f;
			messages.push_front(msg);
		}
	}
	
	bool System::buttonPressed(uint32 port, uint32 unit, uint32 button)
	{
		return (((~ctrlStatus[port][unit].buttons & button) == 0) && ((~lastCtrlStatus[port][unit].buttons & button) != 0));
	}
	
	bool System::buttonHeld(uint32 port, uint32 unit, uint32 button)
	{
		return ((~ctrlStatus[port][unit].buttons & button) == 0);
	}
	
	bool System::isDriveOpen()
	{
		int status, disc_type;
		assert_msg(cdrom_get_status(&status, &disc_type) >= 0, "Failed to get CDROM status");
		return (status == CD_STATUS_OPEN);
	}
	
	void System::doFrame()
	{
		pvr->beginScene();
		
		if(debugEnabled)
		{
			dbgFont->setScale(0.75f);
			dbgFont->setColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
			
			float xPadding = 0.0f;
			float yPos = 472.0f;
			float yHeight = ((signed)messages.size() * dbgFont->getScaledHeight());
			
			pvr->setTexture(PVR_LIST_TR_POLY, NULL);
			pvr->sendPolygonHeader(PVR_LIST_TR_POLY);
			pvr->beginPrimitives(PVR_LIST_TR_POLY);
			pvr->sendVertex03(PLX_VERT,		xPadding,						yPos - yHeight, 10000.0f, 0.0f, 0.0f, 0x80606060, 0);
			pvr->sendVertex03(PLX_VERT,		xPadding,						yPos,			10000.0f, 0.0f, 0.0f, 0x80606060, 0);
			pvr->sendVertex03(PLX_VERT,		xPadding + (640.0f - xPadding),	yPos - yHeight, 10000.0f, 0.0f, 0.0f, 0x80606060, 0);
			pvr->sendVertex03(PLX_VERT_EOS,	xPadding + (640.0f - xPadding),	yPos,			10000.0f, 0.0f, 0.0f, 0x80606060, 0);
			pvr->endPrimitives();
			
			for (std::deque<MessageStruct>::iterator itr = messages.begin(); itr != messages.end(); )
			{
				if((*itr).timer <= 0.0f)
					itr = messages.erase(itr);
				else
				{
					yPos -= dbgFont->getScaledHeight();
					dbgFont->print(this, xPadding, yPos, 10005.0f, (*itr).text);
					(*itr).timer -= 0.01f;
					++itr;
				}
			}
			
			dbgFont->setScale(1.0f);
		}
		
		int p, u;
		maple_device_t *dev;
		
		for (p = 0; p < MAPLE_PORT_COUNT; p++)
		{
			for (u = 0; u < MAPLE_UNIT_COUNT; u++)
			{
				dev = maple_enum_dev(p, u);
				if (dev != NULL)
				{
					mapleFunctionCodes[p][u] = dev->info.functions;
					if(dev->info.functions & MAPLE_FUNC_CONTROLLER)
					{
						lastCtrlStatus[p][u] = ctrlStatus[p][u];
						ctrlStatus[p][u] = *(cont_state_t *)maple_dev_status(dev);
					}
				}
				else
					mapleFunctionCodes[p][u] = 0;
			}
		}
		
		//assert_msg((dev != 0 || (pvr == NULL && sound != NULL)), "Fake error during main processing");
		
		sound->doFrame();
		mgr->doFrame();
		
		pvr->endScene();
	}
	
	void System::shutdown()
	{
		delete dbgFont;
		delete sound;
		delete pvr;
		delete mgr;
	}
}
