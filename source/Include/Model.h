/*------------------------------------------------------------
	INCLUDE/MODEL.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_MODEL_H
#define _INCLUDED_MODEL_H

namespace frenda
{
	class Model
	{
		public:
			static const std::string formatTag;
			
			struct HeaderStruct
			{
				char formatTag[8];
				uint32 textureOffset;
				uint32 geometryOffset;
				char modelName[16];
			};
			
			struct TextureDataStruct
			{
				plx_texture_t *texture;
			};
			
			struct TextureListStruct
			{
				uint32 textureCount;
				uint32 *textureOffsets;
				TextureDataStruct *textures;
			};
			
			struct VertexListStruct
			{
				uint32 vertexCount;
				pvr_vertex_t *vertices;
			};
			
			struct GeometryStruct
			{
				char geometryName[16];
				VertexListStruct *vertexLists;
			};
			
			struct GeometryListStruct
			{
				uint32 geometryCount;
				uint32 *geometryOffsets;
				GeometryStruct *geometry;
			};
			
			HeaderStruct header;
			TextureListStruct textureList;
			GeometryListStruct geometryList;
			
			Model(frenda::System *, std::string);
			~Model();
			
			void load(std::string);
			void render(frenda::PVR *);
			void render(frenda::PVR *, uint32);
			
		private:
			frenda::System *sys;
	};
}
#endif
