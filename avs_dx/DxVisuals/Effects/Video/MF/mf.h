#pragma once
// Media Foundation
#include <mfmediaengine.h>
#include <mfapi.h>

// Moar ATL
#include <atlcom.h>

// Misc. utility
template<class CoClass>
inline HRESULT createInstance( CComPtr<CComObject<CoClass>> &p )
{
	p = nullptr;
	CComObject<CoClass>** pp = &p;
	CHECK( CComObject<CoClass>::CreateInstance( pp ) );
	( *pp )->AddRef();	// Unlike CoCreateInstance, CComObject::CreateInstance creates an object with reference counter = 0
	return S_OK;
}