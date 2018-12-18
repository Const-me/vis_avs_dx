#include "stdafx.h"
#include "enumDisplays.h"

CStringA displayName( const CStringA& name )
{
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

namespace
{
	sDisplayInfo makeDisplayInfo( const MONITORINFOEXA& mi )
	{
		sDisplayInfo res;
		res.deviceName = mi.szDevice;
		res.rectangle = mi.rcMonitor;
		res.isPrimary = !!( mi.dwFlags & MONITORINFOF_PRIMARY );

		DEVMODEA devMode = {};
		devMode.dmSize = sizeof( DEVMODEA );
		if( EnumDisplaySettingsA( mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode ) )
		{
			res.resolution = CSize{ (int)devMode.dmPelsWidth, (int)devMode.dmPelsHeight };
			res.colorDepth = devMode.dmBitsPerPel;
		}
		else
		{
			res.resolution = res.rectangle.Size();
			res.colorDepth = 32;
		}
		return res;
	}

	BOOL __stdcall enumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
	{
		MONITORINFOEXA mi;
		mi.cbSize = sizeof( MONITORINFOEXA );
		if( !GetMonitorInfoA( hMonitor, &mi ) )
			return TRUE;

		if( mi.dwFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
			return TRUE;

		CAtlArray<sDisplayInfo>* arr = reinterpret_cast<CAtlArray<sDisplayInfo>*>( dwData );
		arr->Add( makeDisplayInfo( mi ) );
		return TRUE;
	}
}

void enumDisplays( CAtlArray<sDisplayInfo>& result )
{
	result.RemoveAll();
	EnumDisplayMonitors( nullptr, nullptr, &enumProc, reinterpret_cast<LPARAM>( &result ) );
}

const sDisplayInfo* displayOrDefault( const CAtlArray<sDisplayInfo>& list, CStringA& deviceName )
{
	// Try to find the requested one
	for( size_t i = 0; i < list.GetCount(); i++ )
	{
		if( list[ i ].deviceName == deviceName )
			return &list[ i ];
	}

	// Try to find primary
	for( size_t i = 0; i < list.GetCount(); i++ )
		if( list[ i ].isPrimary )
		{
			deviceName = list[ i ].deviceName;
			return &list[ i ];
		}

	// Try to find any
	if( list.GetCount() > 0 )
	{
		deviceName = list[ 0 ].deviceName;
		return &list[ 0 ];
	}

	// Everything failed :-(
	return nullptr;
}