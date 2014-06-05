/*------------------------------------------------------------
	INCLUDE/FONT.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_FONT_H
#define _INCLUDED_FONT_H

namespace frenda
{
	class System;
	
	class Font
	{
		public:
			static const std::string formatTag;
			
			struct HeaderStruct
			{
				char formatTag[8];
				uint32 characterDataOffset;
				uint32 fontTextureOffset;
				char fontName[16];
				uint16 defaultHeight;
				char unused[14];
			};
			
			struct CharacterDataStruct
			{
				uint32 characterCode;
				uint16 characterWidth;
				uint16 characterHeight;
				float fontXCoord;
				float fontYCoord;
			};
			
			struct CharacterListStruct
			{
				uint32 characterCount;
				CharacterDataStruct *characters;
			};
			
			HeaderStruct header;
			CharacterListStruct characterList;
			plx_texture_t *texture;
			
			Font(frenda::System *, std::string);
			~Font();
			
			void load(std::string);
			void setColorRGBA(float, float, float, float);
			void setScale(float);
			void setDelay(float);
			void print(frenda::System *, float, float, float, const char *, ...);
			void print(frenda::System *, float, float, float, int, int, const char *, ...);
			
			float getScaledHeight()
			{
				return (header.defaultHeight * scale);
			}
			
		private:
			frenda::System *sys;
			
			void printVl(frenda::System *, float, float, float, int, int, const char *, va_list);
			
			float colorA, colorR, colorG, colorB;
			float scale;
	};
}

#endif
