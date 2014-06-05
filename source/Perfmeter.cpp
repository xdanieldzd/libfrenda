/*
   perfmeter.c

   Copyright (C)2003,2004 Dan Potter
*/

/* Adapted from FoF/Tryptonite */

/* Modified & C++-ified 2013 by xdaniel */

#include "include/frenda.h"

namespace frenda
{
	const float Perfmeter::barLength = 200.0f;
	
	Perfmeter::Perfmeter(frenda::System *s)
	{
		sys = s;
		
		sys->debugLog("%s -> initializing perfmeter...\n", __PRETTY_FUNCTION__);
		
		enabled = true;
	}
	
	void Perfmeter::renderBar(float pct, float posx, float posy, float posz, uint32 c1, uint32 c2)
	{
		sys->pvr->sendVertex03(PLX_VERT, posx, posy, posz, 0.0f, 0.0f, c1, 0);
		sys->pvr->sendVertex03(PLX_VERT, posx, posy + 6.0f, posz, 0.0f, 0.0f, c1, 0);
		sys->pvr->sendVertex03(PLX_VERT, posx + barLength * pct, posy, posz, 0.0f, 0.0f, c1, 0);
		sys->pvr->sendVertex03(PLX_VERT_EOS, posx + barLength * pct, posy + 6.0f, posz, 0.0f, 0.0f, c1, 0);
		
		sys->pvr->sendVertex03(PLX_VERT, posx + barLength * pct, posy, posz, 0.0f, 0.0f, c2, 0);
		sys->pvr->sendVertex03(PLX_VERT, posx + barLength * pct, posy + 6.0f, posz, 0.0f, 0.0f, c2, 0);
		sys->pvr->sendVertex03(PLX_VERT, posx + barLength, posy, posz, 0.0f, 0.0f, c2, 0);
		sys->pvr->sendVertex03(PLX_VERT_EOS, posx + barLength, posy + 6.0f, posz, 0.0f, 0.0f, c2, 0);
	}
	
	void Perfmeter::render()
	{
		if(!enabled) return;
		
		sys->pvr->setTexture(PVR_LIST_TR_POLY, NULL);
		sys->pvr->sendPolygonHeader(PVR_LIST_TR_POLY);
		sys->pvr->beginPrimitives(PVR_LIST_TR_POLY);
		
		float pct;
		float posx = 32.0f, posy = 424.0f, posz = 4500.0f;
		
		pvr_stats_t stats;
		pvr_get_stats(&stats);
		
		pct = ((float)stats.buf_last_time) * 60.0f / 1000.0f;
		renderBar(pct, posx, posy, posz, 0xff00c0c0, 0xff007070);
		pct = ((float)stats.reg_last_time) * 60.0f / 1000.0f;
		renderBar(pct, posx, posy + 8.0f, posz, 0xff00c000, 0xff007000);
		pct = ((float)stats.rnd_last_time) * 60.0f / 1000.0f;
		renderBar( pct, posx, posy + 16.0f, posz, 0xffc0c000, 0xff707000);
		
		sys->pvr->endPrimitives();
		
		sys->dbgFont->setColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		sys->dbgFont->print(sys, posx + barLength + 16.0f, posy, posz, "fps %.2f  buf %d  reg %d  rnd %d\n", (double)stats.frame_rate, (int)stats.buf_last_time, (int)stats.reg_last_time, (int)stats.rnd_last_time);
	}
}
