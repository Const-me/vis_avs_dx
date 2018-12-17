#pragma once
#include <atltypes.h>
#include <atlstr.h>

struct sDisplayInfo
{
	CSize resolution;
	int bitsPerPixel;
	CStringA displayId;
};

const sDisplayInfo* getDisplayInfo( int comboboxIndex );

void DDraw_SetFullScreenDx( int fs, const CStringA& monitor, int dbl );

bool DDraw_PickModeDx( const CStringA& monitor, int *w, int *h, int *bpp );