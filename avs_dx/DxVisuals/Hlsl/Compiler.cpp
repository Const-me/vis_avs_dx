#include "stdafx.h"
#include "Compiler.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace Hlsl
{
	struct Target
	{
		const char* const target;
		const char* const entryPoint;
		Target( const char* t, const char* e ) :
			target( t ), entryPoint( e ) { }
	};
	static const std::array<Target, 4> s_targets =
	{
		Target{ "vs_5_0", "vertexShader" },
		Target{ "ps_5_0", "pixelShader" },
		Target{ "gs_5_0", "geometryShader" },
		Target{ "cs_5_0", "computeShader" },
	};

	HRESULT compile( eTarget what, const CStringA& hlsl, std::vector<uint8_t>& binary, CStringA& compilerErrors, Defines* pDefines, Includes* pIncludes )
	{
		// https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile
		ID3DInclude* iInclude = nullptr;
		if( nullptr != pIncludes )
			iInclude = pIncludes;

		std::vector<D3D_SHADER_MACRO> macros;
		Defines::getData( pDefines, macros );

		const Target target = s_targets[ (uint8_t)what ];

		CComPtr<ID3DBlob> blobBinary, blobErrors;
#ifdef NDEBUG
		constexpr UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		constexpr UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		const HRESULT hr = D3DCompile( hlsl, (size_t)hlsl.GetLength(), nullptr, macros.data(), iInclude, target.entryPoint, target.target, flags, 0, &blobBinary, &blobErrors );

		if( nullptr != blobBinary )
		{
			const size_t cb = blobBinary->GetBufferSize();
			binary.resize( cb );
			CopyMemory( binary.data(), blobBinary->GetBufferPointer(), cb );
		}

		if( nullptr != blobErrors )
		{
			const int cb = (int)blobErrors->GetBufferSize();
			CopyMemory( compilerErrors.GetBufferSetLength(cb+1), blobErrors->GetBufferPointer(), cb );
			compilerErrors.ReleaseBuffer( cb );
		}

		return hr;
	}
}