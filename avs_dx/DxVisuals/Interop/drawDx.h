#pragma once
#include <atltypes.h>
#include <atlstr.h>

struct sDisplayInfo
{
	CStringA deviceName;
	CSize resolution;
	int colorDepth;
	bool isPrimary;
	CRect rectangle;
};

const sDisplayInfo* getDisplayInfo( int comboboxIndex );

void DDraw_SetFullScreenDx( int fs, const CStringA& monitor, int dbl );

bool DDraw_PickModeDx( CStringA& monitor, int *w, int *h, int *bpp );

bool DDraw_GetMonitorRectDx( CStringA& monitor, RECT* pRect );