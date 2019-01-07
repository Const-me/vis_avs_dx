#include "stdafx.h"
#include "../../avs/vis_avs/draw.h"
#include "drawDx.h"
#include "Utils/WTL/atlapp.h"
#include "Utils/WTL/atlctrls.h"
#include "enumDisplays.h"
#include "interop.h"

extern HINSTANCE g_hInstance;
extern HWND g_hwnd;

static CComAutoCriticalSection g_cs;

#define LOCK() CSLock __lock( g_cs )

CSize s_size;

int cfg_cancelfs_on_deactivate = 1;
int draw_title_p = 0;

static HRESULT initialize()
{
	// factory = nullptr;
	// CHECK( CreateDXGIFactory1( IID_PPV_ARGS( &factory ) ) );
	return S_OK;
}

int DDraw_Init()
{
	return ( SUCCEEDED( initialize() ) ? 0 : 1 );
}

void DDraw_Quit( void )
{ }

void DDraw_Resize( int w, int h, int dsize )
{
	LOCK();
	s_size = CSize{ w, h };
}
void DDraw_BeginResize( void )
{
}

void DDraw_Enter( int *w, int *h, int **fb1, int **fb2 )
{
	LOCK();
	const CSize sz = getRenderSize();
	*w = sz.cx;
	*h = sz.cy;
}

void DDraw_Exit( int which )
{
}

bool g_fs = false;

void DDraw_SetFullScreen( int fs, int w, int h, int dbl, int bpp )
{
	__debugbreak();
}

void DDraw_SetFullScreenDx( int fs, const CStringA& monitor, int dbl )
{
	g_fs = ( 0 != fs );
}

int DDraw_IsFullScreen( void )
{
	return g_fs;
}

CAtlArray<sDisplayInfo> s_displays;
CAtlMap<int, sDisplayInfo> s_cbItems;

HRESULT enumerateOutputs( WTL::CComboBox& comboBox )
{
	// Unlike original AVS, we never switch display modes. This function populates the combobox with the list of displays instead.
	enumDisplays( s_displays );

	for( size_t i = 0; i < s_displays.GetCount(); i++ )
	{
		const sDisplayInfo& di = s_displays[ i ];
		const CStringA name = displayName( di.deviceName );
		CString val;
		val.Format( L"%S %dx%d@%dBPP", name.operator const char*( ),
			di.resolution.cx, di.resolution.cy, di.colorDepth );
		const int ind = comboBox.AddString( val );
		comboBox.SetItemData( ind, i );
	}

	// Combobox sorts it's items as soon as new ones are inserted, that's why using set/get item data to keep the index in s_displays array.
	s_cbItems.RemoveAll();
	const int count = comboBox.GetCount();
	for( int item = 0; item < count; item++ )
	{
		size_t display = comboBox.GetItemData( item );
		s_cbItems[ item ] = s_displays[ display ];
	}

	return S_OK;
}

const sDisplayInfo* getDisplayInfo( int comboboxIndex )
{
	auto p = s_cbItems.Lookup( comboboxIndex );
	if( nullptr != p )
		return &p->m_value;
	return nullptr;
}

void DDraw_EnumDispModes( HWND comboBox )
{
	WTL::CComboBox cb{ comboBox };
	enumerateOutputs( cb );
}

double DDraw_translatePoint( POINT p, int isY )
{
	double v = 0.0;

	ScreenToClient( g_hwnd, &p );

	if( isY )
	{
		if( s_size.cy > 0 )
			v = p.y / (double)( s_size.cy * 0.5 ) - 1.0;
	}
	else
	{
		if( s_size.cx > 0 )
			v = p.x / (double)( s_size.cx * 0.5 ) - 1.0;
	}

	return v;
}

char statustext[ 256 ];
DWORD statustext_life;
int statustext_len;

void DDraw_SetStatusText( char *text, int life )
{
	strcpy( statustext, text );
	statustext_len = life ? life : 2000;
	statustext_life = 1;
}

int DDraw_IsMode( int w, int h, int bpp )
{
	__debugbreak();
	return 0;
}

int DDraw_PickMode( int *w, int *h, int *bpp )
{
	__debugbreak();
	return 0;
}

bool DDraw_PickModeDx( CStringA& monitor, int *w, int *h, int *bpp )
{
	
	enumDisplays( s_displays );
	auto p = displayOrDefault( s_displays, monitor );
	if( nullptr == p )
		return false;

	if( nullptr != w )
		*w = p->resolution.cx;
	if( nullptr != h )
		*h = p->resolution.cy;
	if( nullptr != bpp )
		*bpp = p->colorDepth;
	return true;
}

bool DDraw_GetMonitorRectDx( CStringA& monitor, RECT* pRect )
{
	enumDisplays( s_displays );
	auto p = displayOrDefault( s_displays, monitor );
	if( nullptr == p )
		return false;

	*pRect = p->rectangle;
	return true;
}