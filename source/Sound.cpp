/*------------------------------------------------------------
	SOUND.CPP - Sound-related functions
 -------------------------------------------------------------*/

#include "include/frenda.h"

namespace frenda
{
	Sound::Sound()
	{
		dbglog(DBG_DEBUG, "%s -> initializing SPU and Ogg Vorbis...\n", __PRETTY_FUNCTION__);
		
		memset(&status, 0, sizeof(SoundStatus));
		
		spu_enable();
		snd_stream_init();
		sndoggvorbis_init();
		thd_sleep(10);
	}
	
	Sound::~Sound()
	{
		sndoggvorbis_stop();
		sndoggvorbis_shutdown();
		snd_stream_shutdown();
		spu_disable();
	}
	
	void Sound::doFrame()
	{
		status.isPlaying = sndoggvorbis_isplaying();
		status.bitrate = sndoggvorbis_getbitrate();
		status.artist = sndoggvorbis_getartist();
		if (!status.artist) status.artist = "[none]";
		status.title = sndoggvorbis_gettitle();
		if (!status.title) status.title = "[none]";
	}
	
	void Sound::playSFX(std::string filename)
	{
		dbglog(DBG_DEBUG, "%s -> playing SFX '%s'...\n", __PRETTY_FUNCTION__, filename.c_str());
		sndoggvorbis_start(filename.c_str(), 0);
	}
	
	void Sound::playBGM(std::string filename)
	{
		dbglog(DBG_DEBUG, "%s -> playing BGM '%s'...\n", __PRETTY_FUNCTION__, filename.c_str());
		sndoggvorbis_start(filename.c_str(), 1);
	}
	
	void Sound::stopPlaying()
	{
		dbglog(DBG_DEBUG, "%s -> stopping sound...\n", __PRETTY_FUNCTION__);
		sndoggvorbis_stop();
	}
}
