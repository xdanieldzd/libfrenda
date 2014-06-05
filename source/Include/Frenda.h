/*------------------------------------------------------------
	INCLUDE/FRENDA.H - DC Support System "Frenda"
 -------------------------------------------------------------*/

#ifndef _INCLUDED_FRENDA_H
#define _INCLUDED_FRENDA_H

#define FRENDATITLE	"DC Support System 'Frenda'"

#define ARRAYLENGTH(a) (sizeof(a) / sizeof(a[0]))

/*------------------------------------------------------------
	DC, KOS, etc. library includes
 -------------------------------------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <deque>

#include <kos.h>
#include <assert.h>
#include <plx/dr.h>
#include <plx/context.h>
#include <plx/prim.h>
#include <plx/font.h>
#include <oggvorbis/sndoggvorbis.h>

/*------------------------------------------------------------
	Frenda includes
 -------------------------------------------------------------*/

#include "Assert.h"
#include "Version.h"

#include "Callback.h"

#include "PVR.h"
#include "Sound.h"
#include "Font.h"
#include "StateManager.h"
#include "System.h"
#include "GameState.h"
#include "Model.h"
#include "Perfmeter.h"
#include "Texture.h"

#endif
