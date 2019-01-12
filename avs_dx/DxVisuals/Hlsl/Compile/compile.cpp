#include "stdafx.h"
#include "compile.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace
{
	const array<const char*, 4> s_targetStrings =
	{
		"cs_5_0",
		"vs_5_0",
		"gs_5_0",
		"ps_5_0",
	};
	inline const char* targetString( eStage t )
	{
		return s_targetStrings[ (uint8_t)t ];
	}

	const array<const char*, 4> s_targetNames =
	{
		"compute",
		"vertex",
		"geometry",
		"pixel",
	};

	class Includes : public ID3DInclude
	{
		const CAtlMap<CStringA, CStringA>& data;

		HRESULT __stdcall Open( D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes ) override
		{
			CStringA key = pFileName;
			key.MakeLower();
			auto p = data.Lookup( key );
			if( nullptr != p )
			{
				*ppData = p->m_value.operator const char*( );
				*pBytes = p->m_value.GetLength();
				return S_OK;
			}
			return E_INVALIDARG;
		}

		HRESULT __stdcall Close( LPCVOID pData ) override { return S_OK; }

	public:
		Includes( const CAtlMap<CStringA, CStringA>& inc ) : data( inc ) { }
	};

	CStringA stringFromBlob( ID3DBlob* blob )
	{
		if( nullptr == blob )
			return "";

		const int cb = (int)blob->GetBufferSize();
		CStringA result;
		CopyMemory( result.GetBufferSetLength( cb + 1 ), blob->GetBufferPointer(), cb );
		result.ReleaseBuffer( cb );
		result.Trim();
		return result;
	}
}

namespace Hlsl
{
	const char* targetName( eStage t )
	{
		return s_targetNames[ (uint8_t)t ];
	}

	HRESULT compile( eStage stage, const CStringA& hlsl, const char* name, const CAtlMap<CStringA, CStringA>& inc, const Defines& def, vector<uint8_t>& dxbc, CStringA* pErrorMessages )
	{
		// https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile

		Includes includes{ inc };

		const char* const target = targetString( stage );
		if( nullptr == target )
		{
			logError( "Invalid shader target 0x%X", (int)stage );
			return E_INVALIDARG;
		}

		const auto macros = def.data();

		CComPtr<ID3DBlob> blobBinary, blobErrors;
#ifdef NDEBUG
		constexpr UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		constexpr UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		const HRESULT hr = D3DCompile( hlsl, (size_t)hlsl.GetLength(), nullptr, macros.data(), &includes, "main", target, flags, 0, &blobBinary, &blobErrors );
		const CStringA errorMessages = stringFromBlob( blobErrors );
		if( pErrorMessages )
			*pErrorMessages = errorMessages;

		if( FAILED( hr ) )
		{
			logError( "Error compiling %s shader %s: %s", targetName( stage ), name, errorMessages.operator const char*() );
			return hr;
		}

		if( nullptr == blobBinary )
		{
			logError( "Error compiling %s shader %s: %s", targetName( stage ), name, errorMessages.operator const char*( ) );
			return E_FAIL;
		}

		const size_t cb = blobBinary->GetBufferSize();
		const uint8_t* pBinary = (const uint8_t*)blobBinary->GetBufferPointer();
		dxbc.assign( pBinary, pBinary + cb );;

		if( errorMessages.GetLength() > 0 )
			logWarning( "Warnings in %s shader %s: %s", targetName( stage ), name, errorMessages.operator const char*( ) );

		return hr;
	}
}