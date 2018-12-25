#include "stdafx.h"
#include "mf.h"

#if dbgLogStuff
#include "magicNumbers.h"

void dbgLogMediaEvent( const char* what, MediaEventType eventType, HRESULT hrStatus )
{
	const CStringA name = mediaEventName( eventType );
	if( S_OK != hrStatus )
	{
		CStringA error;
		error = formatDxMessageA( hrStatus );
		logError( "%s: %s %s", what, cstr( name ), cstr( error ) );
	}

	logDebug( "%s: %s", what, cstr( name ) );
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

void dbgLogMediaType( const char* what, IMFMediaType *mt )
{
	GUID minor;
	if( FAILED( mt->GetGUID( MF_MT_SUBTYPE, &minor ) ) )
		return;
	const CStringA name= videoSubtypeName( minor );
	logDebug( "%s: %s", what, cstr( name ) );
}
#endif