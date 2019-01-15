#include "stdafx.h"
#include "md4.h"
#include <bcrypt.h>
#pragma comment( lib, "Bcrypt.lib" )
#include "PerfMeasure.h"

namespace
{
	// https://docs.microsoft.com/en-us/windows/desktop/SecCNG/creating-a-hash-with-cng

	inline bool NT_SUCCESS( NTSTATUS Status )
	{
		return (NTSTATUS)Status >= 0;
	}

	class AlgHandle
	{
		BCRYPT_ALG_HANDLE h = nullptr;

	public:
		vector<uint8_t> hashObjBuffer;

		HRESULT open()
		{
			NTSTATUS res = BCryptOpenAlgorithmProvider( &h, BCRYPT_MD4_ALGORITHM, nullptr, 0 );
			if( !NT_SUCCESS( res ) )
				return HRESULT_FROM_NT( res );
			DWORD cbObject = 0;
			ULONG cbResult;
			res = BCryptGetProperty( h, BCRYPT_OBJECT_LENGTH, (PUCHAR)&cbObject, 4, &cbResult, 0 );
			if( !NT_SUCCESS( res ) )
				return HRESULT_FROM_NT( res );

			hashObjBuffer.resize( cbObject );

			return S_OK;
		}

		~AlgHandle()
		{
			if( nullptr != h )
			{
				BCryptCloseAlgorithmProvider( h, 0 );
				h = nullptr;
			}
		}

		operator BCRYPT_ALG_HANDLE() const { return h; }
	};

	static AlgHandle s_algo;

	class HashHandleBase
	{
		BCRYPT_HASH_HANDLE h = nullptr;

	protected:

		HRESULT create( bool reusable )
		{
			if( !s_algo )
				CHECK( s_algo.open() );
			NTSTATUS res = BCryptCreateHash( s_algo, &h, (PUCHAR)s_algo.hashObjBuffer.data(), (ULONG)s_algo.hashObjBuffer.size(), nullptr, 0, reusable ? BCRYPT_HASH_REUSABLE_FLAG : 0 );
			if( !NT_SUCCESS( res ) )
				return HRESULT_FROM_NT( res );
			return S_OK;
		}

	public:

		~HashHandleBase()
		{
			if( nullptr != h )
			{
				BCryptDestroyHash( h );
				h = nullptr;
			}
		}

		HRESULT hashData( const void *pv, size_t cb )
		{
			NTSTATUS res = BCryptHashData( h, (PUCHAR)pv, (ULONG)cb, 0 );
			if( !NT_SUCCESS( res ) )
				return HRESULT_FROM_NT( res );
			return S_OK;
		}

		HRESULT finishHash( __m128i* pResult )
		{
			NTSTATUS res = BCryptFinishHash( h, (PUCHAR)pResult, 16, 0 );
			if( !NT_SUCCESS( res ) )
				return HRESULT_FROM_NT( res );
			return S_OK;
		}
	};

	class HashHandle : public HashHandleBase
	{
	public:
		HRESULT create()
		{
			return HashHandleBase::create( false );
		}
	};

	class ReusableHash: public HashHandleBase
	{
	public:
		HRESULT create()
		{
			if( !IsWindows8OrGreater() )
			{
				__debugbreak();
				return HRESULT_FROM_WIN32( ERROR_OLD_WIN_VERSION );
			}
			return HashHandleBase::create( true );
		}
	};

	HRESULT md4( const void *pv, size_t cb, __m128i* pResult )
	{
		// HashHandle hh;
		static ReusableHash hh;
		CHECK( hh.create() );
		CHECK( hh.hashData( pv, cb ) );
		CHECK( hh.finishHash( pResult ) );
		return S_OK;
	}
}

__m128i hashString( const CStringA& str )
{
	// PerfMeasure _pm{ "hashString" };
	__m128i result;
	if( FAILED( md4( str.operator const char*( ), (size_t)str.GetLength(), &result ) ) )
		return _mm_setzero_si128();
	return result;
}

__m128i hashBuffer( const void* pv, size_t cb )
{
	// PerfMeasure _pm{ "hashBuffer" };
	__m128i result;
	if( FAILED( md4( pv, cb, &result ) ) )
		return _mm_setzero_si128();
	return result;
}