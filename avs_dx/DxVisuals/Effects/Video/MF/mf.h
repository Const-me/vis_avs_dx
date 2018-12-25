#pragma once
// Media Foundation
#include <Mfidl.h>
#include <mfobjects.h>
#include <mfapi.h>
#include <Mferror.h>

// Moar ATL
#include <atlcom.h>

// Misc. C++
#include <atomic>

// Misc. utility
template<class CoClass>
inline HRESULT createInstance( CComPtr<CComObject<CoClass>> &p )
{
	p = nullptr;
	CComObject<CoClass>** pp = &p;
	CHECK( CComObject<CoClass>::CreateInstance( pp ) );
	( *pp )->AddRef();
	return S_OK;
}

#define dbgLogStuff 1

#if dbgLogStuff
void dbgLogMediaEvent( const char* what, MediaEventType eventType, HRESULT hrStatus );
void dbgLogMediaEvent( const char* what, IMFMediaEvent *pEvent );
#else
#define dbgLogMediaEvent( ... ) __noop
#endif