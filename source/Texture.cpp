/*------------------------------------------------------------
	TEXTURE.CPP - Frenda texture loader
 -------------------------------------------------------------*/

#include "include/frenda.h"

using namespace std;

namespace frenda
{
	const std::string Texture::formatTag = "FrTexv1";
	
	Texture::Texture(frenda::System *s, std::string filename)
	{
		sys = s;
		
		sys->debugLog("%s -> loading texture '%s'...\n", __PRETTY_FUNCTION__, filename.c_str());
		
		memset(&header, 0, sizeof(HeaderStruct));
		
		load(filename);
	}
	
	Texture::~Texture()
	{
		if(texture != NULL) plx_txr_destroy(texture);
	}
	
	void Texture::load(std::string filename)
	{
		uint8 *data;
		file_t fd = fs_open(filename.c_str(), O_RDONLY);
		data = new uint8[fs_total(fd)];
		fs_read(fd, data, fs_total(fd));
		fs_close(fd);
		
		memcpy(&header, data, sizeof(HeaderStruct));
		
		assert_msg(Texture::formatTag.compare(header.formatTag) == 0, "Wrong texture format OR version");
		
		/* Read texture */
		uint16 *tdata = new uint16[header.byteCount];
		memcpy(tdata, &data[0x18], header.byteCount);
		
		int pvrfmt = -1;
		if(header.format == KOS_IMG_FMT_ARGB1555) pvrfmt = PVR_TXRFMT_ARGB1555;
		else if(header.format == KOS_IMG_FMT_ARGB4444) pvrfmt = PVR_TXRFMT_ARGB4444;
		assert_msg(pvrfmt != -1, "Cannot determine PVR texture format");
		
		sys->debugLog(" -> size %lu*%lu, KOS format %lu, pvr format %i, byteCount %08lx\n", header.width, header.height, header.format, pvrfmt, header.byteCount);
		
		texture = plx_txr_canvas(header.width, header.height, pvrfmt);
		pvr_txr_load_ex(tdata, texture->ptr, texture->w, texture->h, PVR_TXRLOAD_16BPP);
		
		delete[] tdata;
		
		delete[] data;
	}
}
