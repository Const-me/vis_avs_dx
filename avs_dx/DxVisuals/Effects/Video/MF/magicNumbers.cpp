#include "stdafx.h"
#include "magicNumbers.h"
#include <mfapi.h>

#pragma comment( lib, "Rpcrt4.lib" )

const char* mediaEventNameCStr( MediaEventType eventType )
{
#define I2S( T ) case T: return #T
	switch( eventType )
	{
		I2S( MEError );

		I2S( MESessionUnknown );
		I2S( MESessionTopologySet );
		I2S( MESessionTopologiesCleared );
		I2S( MESessionStarted );
		I2S( MESessionPaused );
		I2S( MESessionStopped );
		I2S( MESessionClosed );
		I2S( MESessionEnded );
		I2S( MESessionRateChanged );
		I2S( MESessionScrubSampleComplete );
		I2S( MESessionCapabilitiesChanged );
		I2S( MESessionTopologyStatus );
		I2S( MESessionNotifyPresentationTime );
		I2S( MENewPresentation );

		I2S( MESinkUnknown );
		I2S( MEStreamSinkStarted );
		I2S( MEStreamSinkStopped );
		I2S( MEStreamSinkPaused );
		I2S( MEStreamSinkRateChanged );
		I2S( MEStreamSinkRequestSample );
		I2S( MEStreamSinkMarker );
		I2S( MEStreamSinkPrerolled );
		I2S( MEStreamSinkScrubSampleComplete );
		I2S( MEStreamSinkFormatChanged );
		I2S( MEStreamSinkDeviceChanged );
	}
#undef I2S
	return nullptr;
}

CStringA mediaEventName( MediaEventType eventType )
{
	auto r = mediaEventNameCStr( eventType );
	if( nullptr != r ) return r;
	CStringA res;
	res.Format( "%i", (int)eventType );
	return res;
}

const char* videoSubtypeCStr( const GUID& st )
{
#define G2S( T ) if( st == MFVideoFormat_##T ) return #T
	G2S( Base );
	G2S( RGB32 );
	G2S( ARGB32 );
	G2S( RGB24 );
	G2S( RGB555 );
	G2S( RGB565 );
	G2S( RGB8 );
	G2S( AI44 );
	G2S( AYUV );
	G2S( YUY2 );
	G2S( YVYU );
	G2S( YVU9 );
	G2S( UYVY );
	G2S( NV11 );
	G2S( NV12 );
	G2S( YV12 );
	G2S( I420 );
	G2S( IYUV );
	G2S( Y210 );
	G2S( Y216 );
	G2S( Y410 );
	G2S( Y416 );
	G2S( Y41P );
	G2S( Y41T );
	G2S( Y42T );
	G2S( P210 );
	G2S( P216 );
	G2S( P010 );
	G2S( P016 );
	G2S( v210 );
	G2S( v216 );
	G2S( v410 );
	G2S( MP43 );
	G2S( MP4S );
	G2S( M4S2 );
	G2S( MP4V );
	G2S( WMV1 );
	G2S( WMV2 );
	G2S( WMV3 );
	G2S( WVC1 );
	G2S( MSS1 );
	G2S( MSS2 );
	G2S( MPG1 );
	G2S( DVSL );
	G2S( DVSD );
	G2S( DVHD );
	G2S( DV25 );
	G2S( DV50 );
	G2S( DVH1 );
	G2S( DVC );
	G2S( H264 );
	G2S( MJPG );
	G2S( 420O );
	G2S( HEVC );
	G2S( HEVC_ES );
	G2S( H263 );
	G2S( H264_ES );
	G2S( MPEG2 );
#undef G2S
	return nullptr;
}

CStringA videoSubtypeName( const GUID& st )
{
	auto r = videoSubtypeCStr( st );
	if( nullptr != r ) return r;

	char* us = nullptr;
	UuidToStringA( &st, (RPC_CSTR *)&us );
	CStringA res = us;
	RpcStringFreeA( (RPC_CSTR *)&us );
	return res;
}