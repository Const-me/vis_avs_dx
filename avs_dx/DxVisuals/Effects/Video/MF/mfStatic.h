#pragma once
// #include <mfidl.h>
#include <mfmediaengine.h>

HRESULT mfStartup();

// HRESULT mfSourceResolver( CComPtr<IMFSourceResolver>& resolver );

HRESULT coInit();

HRESULT mfEngineFactory( CComPtr<IMFMediaEngineClassFactory>& factory );