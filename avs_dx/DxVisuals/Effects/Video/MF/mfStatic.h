#pragma once
#include <mfmediaengine.h>

HRESULT mfStartup();

// HRESULT mfSourceResolver( CComPtr<IMFSourceResolver>& resolver );

HRESULT mfEngineFactory( CComPtr<IMFMediaEngineClassFactory>& factory );

HRESULT mfShutdown();