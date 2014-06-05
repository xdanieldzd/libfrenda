/*------------------------------------------------------------
	PVR.CPP - Simplified PVR rendering API
 -------------------------------------------------------------*/

/* DMA rendering -> http://elysianshadows.com/dev/community/tutorials/23-dreamcast/37-dma-rendering-and-store-queues-with-kos-on-the-sega-dreamcast */

#include "include/frenda.h"

namespace frenda
{
	static uint8 op_poly_buf[OP_POLY_VBUFSIZE] __attribute__ ((aligned(32)));
	static uint8 tr_poly_buf[TR_POLY_VBUFSIZE] __attribute__ ((aligned(32)));
	
	static pvr_list_t _current_list = PVR_LIST_OP_POLY;
	static uint32 *_start_ptr = NULL;
	static uint32 *_current_ptr = NULL;
	
	static pvr_poly_cxt_t cxt[5];
	static pvr_poly_hdr_t hdr[5];
	
	PVR::PVR()
	{
		dbglog(DBG_DEBUG, "%s -> initializing PowerVR...\n", __PRETTY_FUNCTION__);
		
		/* Set PVR parameters */
		pvr_init_params_t pvr_params;
		pvr_params.vertex_buf_size = GLOBAL_VBUFSIZE;
		pvr_params.dma_enabled = 1;
		pvr_params.fsaa_enabled = 0;
		pvr_params.opb_sizes[PVR_LIST_OP_POLY] = PVR_BINSIZE_32;
		pvr_params.opb_sizes[PVR_LIST_OP_MOD] = PVR_BINSIZE_0;
		pvr_params.opb_sizes[PVR_LIST_TR_POLY] = PVR_BINSIZE_16;
		pvr_params.opb_sizes[PVR_LIST_TR_MOD] = PVR_BINSIZE_0;
		pvr_params.opb_sizes[PVR_LIST_PT_POLY] = PVR_BINSIZE_0;
		
		/* Try to initialize PVR */
		assert_msg(pvr_init(&pvr_params) >= 0, "Fatal error during PVR init");
		
		/* Set pointers to vertex buffers */
		pvr_set_vertbuf(PVR_LIST_OP_POLY, op_poly_buf, OP_POLY_VBUFSIZE);
		pvr_set_vertbuf(PVR_LIST_TR_POLY, tr_poly_buf, TR_POLY_VBUFSIZE);
		
		/* Set misc. PVR parameters */
		pvr_set_pal_format(PVR_PAL_ARGB1555);
		pvr_set_bg_color(0.0f, 0.0f, 0.0f);
		
		/* Set poly context & headers */
		pvr_poly_cxt_col(&cxt[PVR_LIST_OP_POLY], PVR_LIST_OP_POLY);
		cxt[PVR_LIST_OP_POLY].gen.culling = PVR_CULLING_CW;
		setTextureFilter(PVR_LIST_OP_POLY, Trilinear1);
		pvr_poly_compile(&hdr[PVR_LIST_OP_POLY], &cxt[PVR_LIST_OP_POLY]);
		
		pvr_poly_cxt_col(&cxt[PVR_LIST_TR_POLY], PVR_LIST_TR_POLY);
		cxt[PVR_LIST_TR_POLY].gen.culling = PVR_CULLING_CW;
		setTextureFilter(PVR_LIST_TR_POLY, Trilinear1);
		pvr_poly_compile(&hdr[PVR_LIST_TR_POLY], &cxt[PVR_LIST_TR_POLY]);
	}
	
	PVR::~PVR()
	{
		/* Stop PVR */
		pvr_shutdown();
	}
	
	void PVR::beginScene()
	{
		vid_border_color(255, 0, 0);
		pvr_wait_ready();
		vid_border_color(0, 255, 0);
		pvr_scene_begin();
	}
	
	void PVR::setTexture(pvr_list_t list, plx_texture_t *txr)
	{
		if (txr != NULL)
		{
			memcpy(&cxt[list].txr, &txr->cxt_opaque.txr, sizeof(cxt[list].txr));
			cxt[list].txr.enable = PVR_TEXTURE_ENABLE;
			cxt[list].txr.filter = (int)texFilter;
		}
		else
			cxt[list].txr.enable = PVR_TEXTURE_DISABLE;
		
		pvr_poly_compile(&hdr[list], &cxt[list]);
	}
	
	void PVR::setTextureFilter(pvr_list_t list, PVR::TextureFilter filter)
	{
		texFilter = filter;
	}
	
	void PVR::sendVertexList(pvr_list_t list, pvr_vertex_t *v, int vcount)
	{
		irq_disable();
		
		pvr_list_prim(list, &hdr[list], sizeof(hdr[list]));
		
		pvr_vertex_t *vd = (pvr_vertex_t*)pvr_vertbuf_tail(list);
		QACR0 = ((((uint32)vd) >> 26) << 2) & 0x1C;
		QACR1 = ((((uint32)vd) >> 26) << 2) & 0x1C;
		void *sqd = (void *)(0xE0000000 | (((uint32)vd) & 0x03FFFFE0));
		
		mat_transform_sq(v, sqd, vcount);
		pvr_vertbuf_written(list, 32 * vcount);
		
		irq_enable();
	}
	
	void PVR::sendPolygonHeader(pvr_list_t list)
	{
		pvr_list_prim(list, &hdr[list], sizeof(hdr[list]));
	}
	
	void PVR::beginPrimitives(pvr_list_t list)
	{
		_current_list = list;
		uint32 *ptr = (uint32*)pvr_vertbuf_tail(_current_list);
		QACR0 = ((((uint32)ptr) >> 26) << 2) & 0x1C;
		QACR1 = ((((uint32)ptr) >> 26) << 2) & 0x1C;
		_start_ptr = _current_ptr = (uint32 *)(0xE0000000 | (((uint32)ptr) & 0x03FFFFE0));
	}
	
	void PVR::sendVertex03(uint32 flags, float x, float y, float z, float u, float v, uint32 argb, uint32 oargb)
	{
		register uint32 *_ptr = _current_ptr;
		_ptr += 8;
		*--_ptr = oargb;
		*--_ptr = argb;
		*(float*)--_ptr = v;
		*(float*)--_ptr = u;
		*(float*)--_ptr = z;
		*(float*)--_ptr = y;
		*(float*)--_ptr = x;
		*--_ptr = flags;
		PREFETCH((void *)_ptr);
		_ptr += 8;
		_current_ptr = _ptr;
	}
	
	void PVR::endPrimitives()
	{
		uint32 bytes = (uint32)_current_ptr - (uint32)_start_ptr;
		if (bytes != 0)	pvr_vertbuf_written(_current_list, bytes);
	}
	
	void PVR::endScene()
	{
		vid_border_color(0, 0, 255);
		pvr_scene_finish();
	}
}
