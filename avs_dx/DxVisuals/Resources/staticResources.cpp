#include "stdafx.h"
#include "staticResources.h"
#include "createShaders.hpp"
#include "../Effects/shadersCode.h"

namespace StaticResources
{
	HRESULT create()
	{
		// Static shaders
		using namespace Hlsl::StaticShaders;
		CHECK( createShader( FullScreenTriangleVS(), fullScreenTriangle ) );
		CHECK( createShader( CopyTexturePS(), copyTexture ) );

		// Sampler
		{
			CD3D11_SAMPLER_DESC samplerDesc{ D3D11_DEFAULT };
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			CHECK( device->CreateSamplerState( &samplerDesc, &sampleBilinear ) );
		}

		// Blend state
		{
			CD3D11_BLEND_DESC blendDesc{ D3D11_DEFAULT };
			D3D11_RENDER_TARGET_BLEND_DESC& rt = blendDesc.RenderTarget[ 0 ];

			// https://stackoverflow.com/a/18972920/126995
			rt.BlendEnable = TRUE;
			rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rt.BlendOp = D3D11_BLEND_OP_ADD;
			rt.SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
			rt.DestBlendAlpha = D3D11_BLEND_ONE;
			rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			CHECK( device->CreateBlendState( &blendDesc, &blendPremultipliedAlpha ) );
		}

		return S_OK;
	}
	void destroy()
	{
		fullScreenTriangle = nullptr;
		copyTexture = nullptr;
		sampleBilinear = nullptr;
		blendPremultipliedAlpha = nullptr;
	}

	CComPtr<ID3D11VertexShader> fullScreenTriangle;
	CComPtr<ID3D11PixelShader> copyTexture;
	CComPtr<ID3D11SamplerState> sampleBilinear;
	CComPtr<ID3D11BlendState> blendPremultipliedAlpha;
};