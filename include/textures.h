#ifndef __TEXTURES_H
#define __TEXTURES_H

#include <gsKit.h>

enum INTERNAL_TEXTURE {
	BACKGROUND = 0,
	BACKGROUND2,
	LOGO,

	TEXTURES_COUNT
};

int texPngLoad(GSTEXTURE *texture, int texID);

#endif
