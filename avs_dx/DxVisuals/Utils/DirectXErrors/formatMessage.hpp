#pragma once

namespace
{
	constexpr DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	constexpr DWORD langId = MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT );

	inline DWORD formatMessageA( HRESULT hr, char **ppStr )
	{
		return ::FormatMessageA( flags, nullptr, hr, langId, (LPSTR)ppStr, 0, nullptr );
	}

	inline DWORD formatMessageW( HRESULT hr, wchar_t **ppStr )
	{
		return ::FormatMessageW( flags, nullptr, hr, langId, (LPWSTR)ppStr, 0, nullptr );
	}
}