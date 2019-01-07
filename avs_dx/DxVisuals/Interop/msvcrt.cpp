#include "stdafx.h"
#include "msvcrt.h"

namespace msvcrt
{
	class Dll
	{
		HMODULE hm = nullptr;

		template<class T>
		inline void getProc( T& fn, const char* lpProcName )
		{
			fn = (T)GetProcAddress( hm, lpProcName );
			if( nullptr == fn )
			{
				logError( "%s not found in msvcrt.dll", lpProcName );
				__debugbreak();
			}
		}

	public:
		Dll()
		{
			hm = GetModuleHandleW( L"msvcrt.dll" );
			if( nullptr == hm )
			{
				logError( getLastHr(), "msvcrt.dll wasn't loaded" );
				__debugbreak();
			}
			// getProc( fnMalloc, "malloc" );
			// getProc( fnFree, "free" );
			getProc( fnNew, "??2@YAPAXI@Z" );
			getProc( fnDelete, "??3@YAXPAX@Z" );
		}

		~Dll()
		{
			if( hm )
			{
				FreeLibrary( hm );
				hm = nullptr;
			}
		}

		// void* ( __cdecl *fnMalloc )( size_t size );
		// void( __cdecl *fnFree )( void* ptr );
		void* ( __cdecl *fnNew )( size_t size );
		void( __cdecl *fnDelete )( void* ptr );
	};

	const Dll& dll()
	{
		static const Dll s_dll;
		return s_dll;
	}

	/* void* malloc( size_t size )
	{
		return dll().fnMalloc( size );
	}
	void free( void* ptr )
	{
		return dll().fnFree( ptr );
	} */
	void* operatorNew( size_t size )
	{
		return dll().fnNew( size );
	}
	void operatorDelete( void* ptr )
	{
		dll().fnDelete( ptr );
	}
};
// for EASTL
void* operator new[]( size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line )
{
	return msvcrt::operatorNew( size );
}
void* operator new[]( size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line )
{
	return msvcrt::operatorNew( size );
}
void __cdecl eastl::AssertionFailure( const char *af )
{
	OutputDebugStringA( af );
	__debugbreak();
}

void* operator new[]( size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line );
void* operator new( size_t size )
{
	return msvcrt::operatorNew( size );
}
void* operator new[]( size_t size )
{
	return msvcrt::operatorNew( size );
}
void  operator delete  ( void* ptr )
{
	msvcrt::operatorDelete( ptr );
}
void  operator delete[]( void* ptr )
{
	msvcrt::operatorDelete( ptr );
}