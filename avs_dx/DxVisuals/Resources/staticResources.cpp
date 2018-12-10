#include "stdafx.h"
#include "staticResources.h"
#include "createShaders.hpp"
#include "../Effects/shadersCode.h"

namespace StaticResources
{
	HRESULT create()
	{
		using namespace Hlsl::StaticShaders;
		CHECK( createShader( FullScreenTriangleVS(), fullScreenTriangle ) );
		CHECK( createShader( CopyTexturePS(), copyTexture ) );
		return S_OK;
	}
	void destroy()
	{
		fullScreenTriangle = nullptr;
		copyTexture = nullptr;
	}

	CComPtr<ID3D11VertexShader> fullScreenTriangle;
	CComPtr<ID3D11PixelShader> copyTexture;
};