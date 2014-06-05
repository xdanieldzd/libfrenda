/*------------------------------------------------------------
	INCLUDE/TEXTURE.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_TEXTURE_H
#define _INCLUDED_TEXTURE_H

namespace frenda
{
	class Texture
	{
		public:
			static const std::string formatTag;
			
			struct HeaderStruct
			{
				char formatTag[8];
				uint32 width;
				uint32 height;
				uint32 format;
				uint32 byteCount;
			};
			
			HeaderStruct header;
			plx_texture_t *texture;
			
			Texture(frenda::System *, std::string);
			~Texture();
			
			void load(std::string);
			
		private:
			frenda::System *sys;
	};
}

#endif
