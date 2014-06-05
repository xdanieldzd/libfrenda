/*------------------------------------------------------------
	ASSERT.CPP - Custom assert handler
 -------------------------------------------------------------*/

#include "include/frenda.h"

void assertHandler(const char * file, int line, const char * expr, const char * msg, const char * func)
{
	dbglog(DBG_CRITICAL, "Assertion failed at %s:%d\n", file, line);
	dbglog(DBG_CRITICAL, "Message: %s\n", msg);
	dbglog(DBG_CRITICAL, "Function: %s\n", func);
	dbglog(DBG_CRITICAL, "Condition: %s\n", expr);
	dbglog(DBG_CRITICAL, "-- Execution halted --\n");
	
	/* Clear VRAM w/ nice pattern - \usr\local\dc\kos\kos\examples\dreamcast\video\bfont\bfont.c */
	for (int y = 0; y < 480; y++)
	{
		for (int x = 0; x < 640; x++)
		{
#ifdef DEBUG_ASSERT_FANCYBG
			int c = (x ^ y) & 255;
			vram_s[y * 640 + x] = ((c >> 3) << 12) | ((c >> 2) << 5) | ((c >> 3) << 0);
#else
			vram_s[y * 640 + x] = 0xFF;
#endif
		}
	}
	
	/* Hang with assert display */
	char buf[1024];
	while(1)
	{
		int o = 20 * 640 + 20;
		
		sprintf(buf, "Assertion failed at %s:%d", file, line);
		assertPrintLine(o, buf);
		
		sprintf(buf, "Message: %s", msg);
		assertPrintLine(o, buf);
		o += 640 * 24;
		
		assertPrintLine(o, "Function:");
		assertPrintLine(o, func);
		o += 640 * 24;
		
		assertPrintLine(o, "Condition:");
		assertPrintLine(o, expr);
		o += 640 * 24;
		
		assertPrintLine(o, "-- Execution halted --");
	};
}

void assertPrintLine(int &o, const char * msg)
{
	int idx = 0;
	int tmpstart = 0;
	int tmpidx = 0;
	
	while(msg[idx] != '\0')
	{
		tmpidx++;
		idx++;
		
		if(tmpidx == 50 || msg[idx] == '\0' || msg[idx] == '\n')
		{
			char buf[256];
			memset(buf, 0, sizeof(char) * 256);
			strncpy(buf, &msg[tmpstart], tmpidx);
			bfont_draw_str(vram_s + o, 640, 1, buf);
			o += 640 * 24;
			
			tmpstart += tmpidx;
			tmpidx = 0;
		}
	}
}
