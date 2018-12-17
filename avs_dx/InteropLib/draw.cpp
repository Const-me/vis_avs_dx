#include "stdafx.h"
#include "../../avs/vis_avs/draw.h"
#include "drawDx.h"
#include "Utils/WTL/atlctrls.h"
#pragma comment(lib, "DXGI.lib")

extern HINSTANCE g_hInstance;
extern HWND g_hwnd;

static CComAutoCriticalSection g_cs;

#define LOCK() CSLock __lock( g_cs )

static CComPtr<IDXGIFactory1> factory;

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
{
	factory = nullptr;
}

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
	logError( "DDraw_SetFullScreenDx is not yet implemented" );
}

int DDraw_IsFullScreen( void )
{
	return g_fs;
}

CAtlMap<int, sDisplayInfo> s_displays;

static CStringA displayName( const char* deviceName )
{
	const CStringA name = deviceName;
	const CStringA prefix = R"(\\.\Display)";
	const int prefixLength = prefix.GetLength();
	if( 0 == name.Left( prefixLength ).CompareNoCase( prefix ) )
	{
		const int num = atoi( name.operator const char*( ) + prefixLength );
		CStringA res;
		res.Format( "Display %i", num );
		return res;
	}
	return name;
}

HRESULT enumerateOutputs( WTL::CComboBox& comboBox )
{
	s_displays.RemoveAll();

	// Unlike original AVS, we never switch display modes. This function populates the combobox with the list of displays instead.
	for( DWORD i = 0; true; i++ )
	{
		DISPLAY_DEVICEA displayDevice = {};
		displayDevice.cb = sizeof( DISPLAY_DEVICEA );
		if( !EnumDisplayDevicesA( nullptr, i, &displayDevice, 0 ) )
			return S_FALSE;
		DEVMODEA devMode = {};
		devMode.dmSize = sizeof( DEVMODEA );
		if( !EnumDisplaySettingsA( displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode ) )
			continue;

		const CStringA name = displayName( displayDevice.DeviceName );

		CString val;
		val.Format( L"%S %dx%d@%dBPP", name.operator const char*( ),
			devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel );
		const int ind = comboBox.AddString( val );
		sDisplayInfo di;
		di.displayId = displayDevice.DeviceName;
		di.resolution = CSize{ (int)devMode.dmPelsWidth, (int)devMode.dmPelsHeight };
		di.bitsPerPixel = devMode.dmBitsPerPel;
		s_displays[ ind ] = di;
	}
}

const sDisplayInfo* getDisplayInfo( int comboboxIndex )
{
	auto p = s_displays.Lookup( comboboxIndex );
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

bool DDraw_PickModeDx( const CStringA& monitor, int *w, int *h, int *bpp )
{
	DEVMODEA devMode = {};
	devMode.dmSize = sizeof( DEVMODEA );
	if( EnumDisplaySettingsA( monitor, ENUM_CURRENT_SETTINGS, &devMode ) || EnumDisplaySettingsA( nullptr, ENUM_CURRENT_SETTINGS, &devMode ) )
	{
		*w = (int)devMode.dmPelsWidth;
		*h = (int)devMode.dmPelsHeight;
		*bpp = (int)devMode.dmBitsPerPel;
		return true;
	}
	return false;
}