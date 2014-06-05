/*------------------------------------------------------------
	INCLUDE/PVR.H
 -------------------------------------------------------------*/

#ifndef _INCLUDED_PVR_H
#define _INCLUDED_PVR_H

#define GLOBAL_VBUFSIZE		(2048 * 1024)
#define OP_POLY_VBUFSIZE	(2048 * 1024)
#define TR_POLY_VBUFSIZE	(1024 * 1024)

#define PREFETCH(addr) __asm__ __volatile__("pref @%0" : : "r" (addr))

namespace frenda
{
	class PVR
	{
		public:
			enum TextureFilter
			{
				None = PVR_FILTER_NONE,
				Nearest = PVR_FILTER_NEAREST,
				Bilinear = PVR_FILTER_BILINEAR,
				Trilinear1 = PVR_FILTER_TRILINEAR1,
				Trilinear2 = PVR_FILTER_TRILINEAR2
			};
			
			PVR();
			~PVR();
			void beginScene();
			void setTexture(pvr_list_t list, plx_texture_t *txr);
			void setTextureFilter(pvr_list_t list, TextureFilter filter);
			void sendVertexList(pvr_list_t list, pvr_vertex_t *v, int vcount);
			void sendPolygonHeader(pvr_list_t);
			void beginPrimitives(pvr_list_t);
			void sendVertex03(uint32, float, float, float, float, float, uint32, uint32);
			void endPrimitives();
			void endScene();
			
		private:
			TextureFilter texFilter;
	};
}

#endif
