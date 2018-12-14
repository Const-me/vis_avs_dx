#include "stdafx.h"
#include "staticResources.h"
#include "createShader.hpp"
#include "../Effects/shadersCode.h"

namespace StaticResources
{
	HRESULT create()
	{
		// Static shaders
		using namespace Hlsl::StaticShaders;
		CHECK( createShader( FullScreenTriangleVS(), fullScreenTriangle ) );
		CHECK( createShader( CopyTexturePS(), copyTexture ) );
		CHECK( createShader( PointSpritePS(), pointSprite ) );

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

		// Input layout
		/* {
			static const D3D11_INPUT_ELEMENT_DESC iaDesc[ 2 ] =
			{
				{ "SV_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
		} */

		// Black texture
		{
			CD3D11_TEXTURE2D_DESC texDesc{ DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE };
			const uint32_t data = 0;	// premultiplied alpha
			D3D11_SUBRESOURCE_DATA srd{ &data, 4, 0 };
			CComPtr<ID3D11Texture2D> tex;
			CHECK( device->CreateTexture2D( &texDesc, &srd, &tex ) );

			CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM };
			CHECK( device->CreateShaderResourceView( tex, &srvDesc, &blackTexture ) );
		}

		CHECK( sourceData.create() );

		return S_OK;
	}

	void destroy()
	{
		fullScreenTriangle = nullptr;
		copyTexture = nullptr;
		pointSprite = nullptr;

		sampleBilinear = nullptr;
		blendPremultipliedAlpha = nullptr;
		// layoutPos2Tc2 = nullptr;
		blackTexture = nullptr;
		sourceData.destroy();
	}

	CComPtr<ID3D11VertexShader> fullScreenTriangle;
	CComPtr<ID3D11PixelShader> copyTexture;
	CComPtr<ID3D11PixelShader> pointSprite;

	CComPtr<ID3D11SamplerState> sampleBilinear;
	CComPtr<ID3D11BlendState> blendPremultipliedAlpha;
	CComPtr<ID3D11ShaderResourceView> blackTexture;
	// CComPtr<ID3D11InputLayout> layoutPos2Tc2;
	SourceData sourceData;
};