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
			hm = LoadLibraryW( L"msvcrt.dll" );
			if( nullptr == hm )
			{
				logError( getLastHr(), "Load msvcrt.dll" );
				__debugbreak();
			}
			getProc( fnMalloc, "malloc" );
			getProc( fnFree, "free" );
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

		void* ( __cdecl *fnMalloc )( size_t size );
		void( __cdecl *fnFree )( void* ptr );
		void* ( __cdecl *fnNew )( size_t size );
		void( __cdecl *fnDelete )( void* ptr );
	};

	static const Dll g_dll;

	void* malloc( size_t size )
	{
		return g_dll.fnMalloc( size );
	}
	void free( void* ptr )
	{
		g_dll.fnFree( ptr );
	}

	void* operatorNew( size_t size )
	{
		return g_dll.fnNew( size );
	}

	void operatorDelete( void* ptr )
	{
		g_dll.fnDelete( ptr );
	}
};