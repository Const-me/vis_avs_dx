#include "stdafx.h"
#include "msvcrt.h"

namespace msvcrt
{
	class Dll
	{
		HMODULE hm = nullptr;

		template<class T>
		inline bool getProc( T& fn, const char* lpProcName )
		{
			fn = (T)GetProcAddress( hm, lpProcName );
			if( nullptr == fn )
			{
				char buff[ 64 ];
				sprintf_s( buff, "%s not found in msvcrt.dll\n", lpProcName );
				OutputDebugStringA( buff );
				return false;
			}
			return true;
		}

	public:

		Dll()
		{
			hm = LoadLibraryA( "msvcrt.dll" );
			if( nullptr == hm )
			{
				const HRESULT hr = getLastHr();
				char buff[ 64 ];
				sprintf_s( buff, "msvcrt.dll wasn't loaded, error 0x%08X\n", hr );
				OutputDebugStringA( buff );
				return;
			}
			getProc( fnNew, "??2@YAPAXI@Z" );
			getProc( fnDelete, "??3@YAXPAX@Z" );
		}

		bool startup() const
		{
			return hm && fnNew && fnDelete;
		}

		~Dll()
		{
			fnNew = nullptr;
			fnDelete = nullptr;
			if( hm )
			{
				FreeLibrary( hm );
				hm = nullptr;
			}
		}

		void* ( __cdecl *fnNew )( size_t size ) = nullptr;
		void( __cdecl *fnDelete )( void* ptr ) = nullptr;
	};

	const Dll& dll()
	{
		static const Dll s_dll;
		return s_dll;
	}

	void* operatorNew( size_t size )
	{
		return dll().fnNew( size );
	}
	void operatorDelete( void* ptr )
	{
		dll().fnDelete( ptr );
	}

	bool startup()
	{
		return dll().startup();
	}
};