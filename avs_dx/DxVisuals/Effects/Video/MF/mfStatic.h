#pragma once
#include <mfidl.h>

HRESULT mfStartup();

HRESULT mfSourceResolver( CComPtr<IMFSourceResolver>& resolver );