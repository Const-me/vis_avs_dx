#pragma once

// Get error string from HRESULT code, e.g. "DXGI_ERROR_MORE_DATA". Returns nullptr for unknown codes. The data includes a subset of Win32 errors.
const char* getDxErrorStringA( HRESULT hr );
// (Not recommended) Get error string from HRESULT code, e.g. L"DXGI_ERROR_MORE_DATA". Returns nullptr for unknown codes. The data includes a subset of Win32 errors.
const wchar_t* getDxErrorStringW( HRESULT hr );

// Get error description from HRESULT code, e.g. "The caller did not supply a sufficiently large buffer." Returns nullptr for unknown codes. The data doesn't include Win32 errors, FormatMessage API already knows them.
const char* getDxErrorDescriptionA( HRESULT hr );
// (Not recommended) Get error description from HRESULT code, e.g. L"The caller did not supply a sufficiently large buffer." Returns nullptr for unknown codes. The data doesn't include Win32 errors, FormatMessage API already knows them.
const wchar_t* getDxErrorDescriptionW( HRESULT hr );

#ifdef DXERR_ATL_STRING

CStringA formatDxMessageA( HRESULT hr );
CStringW formatDxMessageW( HRESULT hr );

#elif defined DXERR_STD_STRING

#include <string>
std::string formatDxMessageA( HRESULT hr );
std::wstring formatDxMessageW( HRESULT hr );

#endif