/*------------------------------------------------------------
	FONT.CPP - Text printing
 -------------------------------------------------------------*/

#include "include/frenda.h"

namespace frenda
{
	const std::string Font::formatTag = "FrFntv1";
	
	Font::Font(frenda::System *s, std::string filename)
	{
		sys = s;
		
		sys->debugLog("%s -> loading font '%s'...\n", __PRETTY_FUNCTION__, filename.c_str());
		
		memset(&header, 0, sizeof(HeaderStruct));
		memset(&characterList, 0, sizeof(CharacterListStruct));
		texture = NULL;
		
		load(filename);
		
		colorA = colorR = colorG = colorB = 1.0f;
		scale = 0.5f;
	}
	
	Font::~Font()
	{
		if(texture != NULL) plx_txr_destroy(texture);
		delete[] characterList.characters;
	}
	
	void Font::load(std::string filename)
	{
		uint8 *data;
		file_t fd = fs_open(filename.c_str(), O_RDONLY);
		data = new uint8[fs_total(fd)];
		fs_read(fd, data, fs_total(fd));
		fs_close(fd);
		
		uint32 rofs;
		memcpy(&header, data, sizeof(HeaderStruct));
		
		assert_msg(Font::formatTag.compare(header.formatTag) == 0, "Wrong font format OR version");
		
		/* Read characters */
		rofs = header.characterDataOffset;
		memcpy(&characterList.characterCount, &data[rofs], 4);
		rofs += 4;
		
		characterList.characters = new CharacterDataStruct[characterList.characterCount];
		memcpy(characterList.characters, &data[rofs], (sizeof(CharacterDataStruct) * characterList.characterCount));
		
		/* Read font texture */
		rofs = header.fontTextureOffset;
		int w = 0, h = 0, fmt = 0, byte_count = 0;
		memcpy(&w, &data[rofs], 4);
		memcpy(&h, &data[rofs + 0x4], 4);
		memcpy(&fmt, &data[rofs + 0x8], 4);
		memcpy(&byte_count, &data[rofs + 0xC], 4);
		
		uint32 *tdata = new uint32[byte_count];
		memcpy(tdata, &data[rofs + 0x10], byte_count);
		
		texture = plx_txr_canvas(w, h, PVR_TXRFMT_ARGB4444);
		pvr_txr_load_ex(tdata, texture->ptr, texture->w, texture->h, PVR_TXRLOAD_16BPP);
		
		delete[] tdata;
		
		delete[] data;
	}
	
	void Font::setColorRGBA(float r, float g, float b, float a)
	{
		colorA = a;
		colorR = r;
		colorG = g;
		colorB = b;
	}
	
	void Font::setScale(float s)
	{
		scale = s;
	}
	
	void Font::print(frenda::System *sys, float x, float y, float z, const char *fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		Font::printVl(sys, x, y, z, -1, -1, fmt, vl);
		va_end(vl);
	}
	
	void Font::print(frenda::System *sys, float x, float y, float z, int start, int printLen, const char *fmt, ...)
	{
		va_list vl;
		va_start(vl, fmt);
		Font::printVl(sys, x, y, z, start, printLen, fmt, vl);
		va_end(vl);
	}
	
	void Font::printVl(frenda::System *sys, float x, float y, float z, int start, int printLen, const char *fmt, va_list vl)
	{
		unsigned char buf[1024];
		vsnprintf((char*)buf, sizeof(buf), fmt, vl);
		
		if(start == -1) start = 0;
		if(printLen == -1) printLen = strlen((char*)buf);
		
		sys->pvr->setTexture(PVR_LIST_TR_POLY, texture);
		sys->pvr->sendPolygonHeader(PVR_LIST_TR_POLY);
		sys->pvr->beginPrimitives(PVR_LIST_TR_POLY);
		
		float cr = colorR, cg = colorG, cb = colorB, ca = colorA;
		float cx = x, cy = y;
		
		// TODO alpha not applied during rendering?!
		
		for(int i = start; i < start + printLen; i++)
		{
			if((buf[i] == '{' && buf[i + 1] == 'n') || buf[i] == '\n')
			{
				cx = x;
				cy += getScaledHeight();
				if(buf[i] != '\n') i += 2;
			}
			else if(buf[i] == '{')
			{
				if(buf[i + 1] == 'c')
				{
					if(buf[i + 2] == '}')
					{
						cr = colorR;
						cg = colorG;
						cb = colorB;
						i += 2;
					}
					else
					{
						int r, g, b;
						sscanf((char*)&buf[i + 2], "%02x%02x%02x", &r, &g, &b);
						cr = ((unsigned char)r) / 255.0f;
						cg = ((unsigned char)g) / 255.0f;
						cb = ((unsigned char)b) / 255.0f;
						i += 8;
					}
				}
				else if(buf[i + 1] == 'a')
				{
					if(buf[i + 2] == '}')
					{
						ca = colorA;
						i += 2;
					}
					else
					{
						int a;
						sscanf((char*)&buf[i + 2], "%02x", &a);
						ca = ((unsigned char)a) / 255.0f;
						i += 4;
					}
				}
				else if(buf[i + 1] == 'p')
				{
					break;
				}
			}
			else
			{
				CharacterDataStruct *chdata = NULL;
				
				for(uint32 j = 0; j < characterList.characterCount; j++)
				{
					if(characterList.characters[j].characterCode == (uint32)buf[i])
					{
						chdata = &characterList.characters[j];
						break;
					}
				}
				
				if(chdata)
				{
					if(chdata->characterCode != 0x20)
					{
						float w = chdata->characterWidth, h = chdata->characterHeight;
						float u1 = (chdata->fontXCoord / texture->w);
						float u2 = ((chdata->fontXCoord + w) / texture->w);
						float v1 = (chdata->fontYCoord / texture->h);
						float v2 = ((chdata->fontYCoord + h) / texture->h);
						w *= scale;
						h *= scale;
						uint32 col = PVR_PACK_COLOR(ca, cr, cg, cb);
						sys->pvr->sendVertex03(PLX_VERT, cx, cy, z, u1, v1, col, 0);
						sys->pvr->sendVertex03(PLX_VERT, cx, cy + h, z, u1, v2, col, 0);
						sys->pvr->sendVertex03(PLX_VERT, cx + w, cy, z, u2, v1, col, 0);
						sys->pvr->sendVertex03(PLX_VERT_EOS, cx + w, cy + h, z, u2, v2, col, 0);
						cx += w;
					}
					else
						cx += (chdata->characterWidth * scale);
				}
				else
					cx += (12.0f * scale);
			}
		}
		
		sys->pvr->endPrimitives();
		
		sys->pvr->setTexture(PVR_LIST_TR_POLY, NULL);
		sys->pvr->sendPolygonHeader(PVR_LIST_TR_POLY);
	}
}
