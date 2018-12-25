#include "stdafx.h"
#include "mf.h"

#if dbgLogStuff

const char* mediaEventName( MediaEventType eventType )
{
	if( !dbgLogStuff )
		return nullptr;

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

void dbgLogMediaEvent( const char* what, MediaEventType eventType, HRESULT hrStatus )
{
	const char* const ts = mediaEventName( eventType );
	if( S_OK != hrStatus )
	{
		CStringA error;
		error = formatDxMessageA( hrStatus );

		if( nullptr != ts )
			logError( "%s: %s %s", what, ts, cstr( error ) );
		else
			logError( "%s: %i %s", what, (int)eventType, cstr( error ) );
	}
	if( nullptr != ts )
		logDebug( "%s: %s", what, ts );
	else
		logDebug( "%s: %i", what, (int)eventType );
}

void dbgLogMediaEvent( const char* what, IMFMediaEvent *pEvent )
{
	if( nullptr == pEvent )
		return;

	MediaEventType met;
	if( FAILED( pEvent->GetType( &met ) ) )
		return;

	HRESULT hrStatus = E_FAIL;
	if( FAILED( pEvent->GetStatus( &hrStatus ) ) )
		return;
	dbgLogMediaEvent( what, met, hrStatus );
}

#endif