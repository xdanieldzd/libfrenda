/*------------------------------------------------------------
	INCLUDE/SOUND.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_SOUND_H
#define _INCLUDED_SOUND_H

namespace frenda
{
	class Sound
	{
		public:
			struct SoundStatus
			{
				bool isPlaying;
				long bitrate;
				char *artist, *title;
			};
			
			SoundStatus status;
			
			Sound();
			~Sound();
			void doFrame();
			void playSFX(std::string);
			void playBGM(std::string);
			void stopPlaying();
	};
}

#endif
