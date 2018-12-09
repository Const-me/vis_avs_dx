#include "stdafx.h"
#include "Compiler.h"
#include "../Effects/shadersCode.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace
{
	static const char* targetString( eTarget t )
	{
		switch( t )
		{
		case eTarget::Vertex: return "vs_5_0";
		case eTarget::Pixel: return "ps_5_0";
		case eTarget::Geometry: return "gs_5_0";
		case eTarget::Compute: return "cs_5_0";
		}
		return nullptr;
	}

	class Includes : public ID3DInclude
	{
		const CAtlMap<CStringA, CStringA>& data = Hlsl::includes();

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
	};
}

namespace Hlsl
{
	HRESULT compile( eTarget what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines, Includes* pIncludes )
	{
		// https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile
		ID3DInclude* iInclude = nullptr;
		if( nullptr != pIncludes )
			iInclude = pIncludes;

		const char* const target = targetString( what );
		if( nullptr == target )
		{
			logError( "Invalid shader target 0x%X", (int)what );
			return E_INVALIDARG;
		}

		std::vector<D3D_SHADER_MACRO> macros;
		Defines::getData( pDefines, macros );

		CComPtr<ID3DBlob> blobBinary, blobErrors;
#ifdef NDEBUG
		constexpr UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		constexpr UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		const HRESULT hr = D3DCompile( hlsl, (size_t)hlsl.GetLength(), nullptr, macros.data(), iInclude, "main", target, flags, 0, &blobBinary, &blobErrors );
		if( FAILED( hr ) )
			logWarning( hr, "D3DCompile" );

		if( nullptr != blobBinary )
		{
			const size_t cb = blobBinary->GetBufferSize();
			binary.resize( cb );
			CopyMemory( binary.data(), blobBinary->GetBufferPointer(), cb );
		}

		if( nullptr != blobErrors )
		{
			const int cb = (int)blobErrors->GetBufferSize();
			CopyMemory( compilerErrors.GetBufferSetLength( cb + 1 ), blobErrors->GetBufferPointer(), cb );
			compilerErrors.ReleaseBuffer( cb );
		}
		return hr;
	}
}