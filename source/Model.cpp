/*------------------------------------------------------------
	MODEL.CPP - "Frenda 3D" model loader
 -------------------------------------------------------------*/

#include "include/frenda.h"

using namespace std;

namespace frenda
{
	const std::string Model::formatTag = "Fr3Dv04";
	
	Model::Model(frenda::System *s, std::string filename)
	{
		sys = s;
		
		sys->debugLog("%s -> loading model '%s'...\n", __PRETTY_FUNCTION__, filename.c_str());
		
		memset(&header, 0, sizeof(HeaderStruct));
		memset(&textureList, 0, sizeof(TextureListStruct));
		memset(&geometryList, 0, sizeof(GeometryListStruct));
		
		load(filename);
	}
	
	Model::~Model()
	{
		for(uint32 i = 0; i < geometryList.geometryCount; i++)
		{
			//sys->debugLog(" -> unloading geometry %lu/%lu '%s'...\n", (i + 1), geometryList.geometryCount, geometryList.geometry[i].geometryName);
			for(uint32 j = 0; j < textureList.textureCount; j++)
			{
				//sys->debugLog("  -> unloading vertex list %lu/%lu\n", (j + 1), textureList.textureCount);
				delete[] geometryList.geometry[i].vertexLists[j].vertices;
			}
			delete[] geometryList.geometry[i].vertexLists;
		}
		delete[] geometryList.geometry;
		delete[] geometryList.geometryOffsets;
		
		for(uint32 i = 0; i < textureList.textureCount; i++)
		{
			if(textureList.textures[i].texture != NULL) plx_txr_destroy(textureList.textures[i].texture);
		}
		delete[] textureList.textures;
		delete[] textureList.textureOffsets;
	}
	
	void Model::load(std::string filename)
	{
		uint8 *data;
		file_t fd = fs_open(filename.c_str(), O_RDONLY);
		data = new uint8[fs_total(fd)];
		fs_read(fd, data, fs_total(fd));
		fs_close(fd);
		
		uint32 rofs;
		memcpy(&header, data, sizeof(HeaderStruct));
		
		assert_msg(Model::formatTag.compare(header.formatTag) == 0, "Wrong model format OR version");
		
		/* Read textures */
		rofs = header.textureOffset;
		memcpy(&textureList.textureCount, &data[rofs], 4);
		rofs += 4;
		
		textureList.textureOffsets = new uint32[textureList.textureCount];
		memcpy(textureList.textureOffsets, &data[rofs], (sizeof(uint32) * textureList.textureCount));
		textureList.textures = new TextureDataStruct[textureList.textureCount];
		
		for(uint32 i = 0; i < textureList.textureCount; i++)
		{
			//sys->debugLog(" -> loading texture %lu/%lu from 0x%08lx\n", (i + 1), textureList.textureCount, textureList.textureOffsets[i]);
			
			rofs = textureList.textureOffsets[i];
			int w = 0, h = 0, fmt = 0, byte_count = 0;
			
			memcpy(&w, &data[rofs], 4);
			memcpy(&h, &data[rofs + 0x4], 4);
			memcpy(&fmt, &data[rofs + 0x8], 4);
			memcpy(&byte_count, &data[rofs + 0xC], 4);
			
			uint16 *tdata = new uint16[byte_count];
			memcpy(tdata, &data[rofs + 0x10], byte_count);
			
			TextureDataStruct *t = &textureList.textures[i];
			
			int pvrfmt = -1;
			if(fmt == KOS_IMG_FMT_ARGB1555) pvrfmt = PVR_TXRFMT_ARGB1555;
			else if(fmt == KOS_IMG_FMT_ARGB4444) pvrfmt = PVR_TXRFMT_ARGB4444;
			assert_msg(pvrfmt != -1, "Cannot determine PVR texture format");
			
			t->texture = plx_txr_canvas(w, h, pvrfmt);
			pvr_txr_load_ex(tdata, t->texture->ptr, t->texture->w, t->texture->h, PVR_TXRLOAD_16BPP);
			
			delete[] tdata;
		}
		
		/* Read geometry */
		rofs = header.geometryOffset;
		memcpy(&geometryList.geometryCount, &data[rofs], 4);
		rofs += 4;
		
		geometryList.geometryOffsets = new uint32[geometryList.geometryCount];
		memcpy(geometryList.geometryOffsets, &data[rofs], (sizeof(uint32) * geometryList.geometryCount));
		geometryList.geometry = new GeometryStruct[geometryList.geometryCount];
		
		for(uint32 i = 0; i < geometryList.geometryCount; i++)
		{
			rofs = geometryList.geometryOffsets[i];
			memcpy(&geometryList.geometry[i].geometryName, &data[rofs], 16);
			rofs += 16;
			
			//sys->debugLog(" -> loading geometry %lu/%lu '%s' from 0x%08lx...\n", (i + 1), geometryList.geometryCount, geometryList.geometry[i].geometryName, geometryList.geometryOffsets[i]);
			
			geometryList.geometry[i].vertexLists = new VertexListStruct[textureList.textureCount];
			for(uint32 j = 0; j < textureList.textureCount; j++)
			{
				memcpy(&geometryList.geometry[i].vertexLists[j].vertexCount, &data[rofs], 4);
				rofs += 4;
				
				//sys->debugLog("  -> loading vertex list %lu/%lu, %lu vertices\n", (j + 1), textureList.textureCount, geometryList.geometry[i].vertexLists[j].vertexCount);
				
				geometryList.geometry[i].vertexLists[j].vertices = new pvr_vertex_t[geometryList.geometry[i].vertexLists[j].vertexCount];
				if(geometryList.geometry[i].vertexLists[j].vertexCount > 0)
				{
					memcpy(geometryList.geometry[i].vertexLists[j].vertices, &data[rofs], (sizeof(pvr_vertex_t) * geometryList.geometry[i].vertexLists[j].vertexCount));
					rofs += (sizeof(pvr_vertex_t) * geometryList.geometry[i].vertexLists[j].vertexCount);
				}
			}
		}
		
		delete[] data;
	}
	
	void Model::render(frenda::PVR *pvr)
	{
		for(uint32 i = 0; i < geometryList.geometryCount; i++)
		{
			render(pvr, i);
		}
	}
	
	void Model::render(frenda::PVR *pvr, uint32 geometryID)
	{
		for(uint32 i = 0; i < textureList.textureCount; i++)
		{
			if(geometryList.geometry[geometryID].vertexLists[i].vertexCount > 0)
			{
				pvr->setTexture(PVR_LIST_TR_POLY, textureList.textures[i].texture);
				pvr->sendVertexList(PVR_LIST_TR_POLY, geometryList.geometry[geometryID].vertexLists[i].vertices, (int)geometryList.geometry[geometryID].vertexLists[i].vertexCount);
			}
		}
	}
}
