/*------------------------------------------------------------
	INCLUDE/ASSERT.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_ASSERT_H
#define _INCLUDED_ASSERT_H

/* Override assert defines from \usr\local\dc\kos\kos\include\assert.h to include __PRETTY_FUNCTION__ (because I like it!) */

#ifdef assert
#undef assert
#endif

#ifdef assert_msg
#undef assert_msg
#endif

#ifdef NDEBUG
#	define assert(e) ((void)0)
#	define assert_msg(e, m) ((void)0)
#else
#	define assert(e)        ((e) ? (void)0 : __assert(__FILE__, __LINE__, #e, NULL, __PRETTY_FUNCTION__))
#	define assert_msg(e, m) ((e) ? (void)0 : __assert(__FILE__, __LINE__, #e, m, __PRETTY_FUNCTION__))
#endif

void assertHandler(const char * file, int line, const char * expr, const char * msg, const char * func);
void assertPrintLine(int &o, const char * msg);

#endif
