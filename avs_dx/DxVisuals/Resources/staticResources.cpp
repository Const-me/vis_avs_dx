#include "stdafx.h"
#include "staticResources.h"
#include "createShader.hpp"
#include "../Effects/shadersCode.h"

namespace StaticResources
{
	static CAtlMap<const void*, CComPtr<ID3D11InputLayout>> s_layoutsCache;

	HRESULT create()
	{
		// Static shaders
		using namespace Hlsl::StaticShaders;
		CHECK( createShader( FullScreenTriangleVS(), fullScreenTriangle ) );
		CHECK( createShader( FullScreenTriangleWithTC(), fullScreenTriangleTC ) );

		CHECK( createShader( CopyTexturePS(), copyTexture ) );
		CHECK( createShader( FadeFramePS(), fadeFramePS ) );

		// Sampler
		{
			CD3D11_SAMPLER_DESC samplerDesc{ D3D11_DEFAULT };
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			CHECK( device->CreateSamplerState( &samplerDesc, &sampleBilinear ) );

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			CHECK( device->CreateSamplerState( &samplerDesc, &sampleNearest ) );
		}

		// Blend states
		{
			CD3D11_BLEND_DESC blendDesc{ D3D11_DEFAULT };
			D3D11_RENDER_TARGET_BLEND_DESC& rt = blendDesc.RenderTarget[ 0 ];

			// https://stackoverflow.com/a/18972920/126995
			rt.BlendEnable = TRUE;
			rt.SrcBlend = D3D11_BLEND_ONE;	// Already premultiplied
			rt.BlendOp = D3D11_BLEND_OP_ADD;
			rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
			CHECK( device->CreateBlendState( &blendDesc, &blendPremultipliedAlpha ) );

			rt.DestBlend = D3D11_BLEND_ONE;
			CHECK( device->CreateBlendState( &blendDesc, &blendAdditive ) );

			rt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			rt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			CHECK( device->CreateBlendState( &blendDesc, &blendCustom ) );
		}

		{
			CD3D11_RASTERIZER_DESC rs{ D3D11_DEFAULT };
			rs.CullMode = D3D11_CULL_NONE;
			CHECK( device->CreateRasterizerState( &rs, &rsDisableCulling ) );
		}

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

		// Bind them
		sourceData.bind<eStage::Compute>();
		sourceData.bind<eStage::Vertex>();
		sourceData.bind<eStage::Geometry>();
		sourceData.bind<eStage::Pixel>();

		bindSampler( 0, sampleBilinear );
		bindSampler( 1, sampleNearest );

		return S_OK;
	}

	void destroy()
	{
		logShutdown( "StaticResources::destroy" );

		fullScreenTriangle = nullptr;
		copyTexture = nullptr;
		fullScreenTriangleTC = nullptr;
		copyTextureBilinear = nullptr;
		fadeFramePS = nullptr;

		sampleBilinear = sampleNearest = nullptr;

		blendPremultipliedAlpha = nullptr;
		blendAdditive = nullptr;
		blendCustom = nullptr;

		blackTexture = nullptr;
		rsDisableCulling = nullptr;
		layoutPos2Tc2 = nullptr;
		s_layoutsCache.RemoveAll();

		sourceData.destroy();
		globalBuffers.destroy();
		globalBlendModes.destroy();
	}

	CComPtr<ID3D11VertexShader> fullScreenTriangle;
	CComPtr<ID3D11PixelShader> copyTexture;
	CComPtr<ID3D11VertexShader> fullScreenTriangleTC;
	CComPtr<ID3D11PixelShader> copyTextureBilinear;
	CComPtr<ID3D11PixelShader> fadeFramePS;

	CComPtr<ID3D11SamplerState> sampleBilinear, sampleNearest;

	CComPtr<ID3D11BlendState> blendPremultipliedAlpha;
	CComPtr<ID3D11BlendState> blendAdditive;
	CComPtr<ID3D11BlendState> blendCustom;

	CComPtr<ID3D11ShaderResourceView> blackTexture;
	CComPtr<ID3D11RasterizerState> rsDisableCulling;
	CComPtr<ID3D11InputLayout> layoutPos2Tc2;

	SourceData sourceData;
	GlobalBuffers globalBuffers;
	BlendModes globalBlendModes;

	static CComAutoCriticalSection s_layoutsLock;

	HRESULT createLayout( const vector<uint8_t>& dxbc )
	{
		if( nullptr != layoutPos2Tc2 )
			return S_FALSE;

		const D3D11_INPUT_ELEMENT_DESC iaDesc[ 2 ] =
		{
			{ "SV_Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		CComPtr<ID3D11InputLayout> layout;
		CHECK( device->CreateInputLayout( iaDesc, 2, dxbc.data(), dxbc.size(), &layout ) );

		CSLock __lock{ s_layoutsLock };
		layoutPos2Tc2 = layout;
		return S_OK;
	}

	HRESULT cacheInputLayout( const void* key, const D3D11_INPUT_ELEMENT_DESC *desc, UINT count, const vector<uint8_t>& dxbc )
	{
		{
			CSLock __lock{ s_layoutsLock };
			if( nullptr != s_layoutsCache.Lookup( key ) )
				return S_FALSE;
		}

		CComPtr<ID3D11InputLayout> layout;
		CHECK( device->CreateInputLayout( desc, count, dxbc.data(), dxbc.size(), &layout ) );

		{
			CSLock __lock{ s_layoutsLock };
			if( nullptr != s_layoutsCache.Lookup( key ) )
				return S_FALSE;
			s_layoutsCache[ key ] = layout;
		}

		return S_OK;
	}

	HRESULT bindCachedInputLayout( const void* key )
	{
		auto p = s_layoutsCache.Lookup( key );
		if( nullptr == p )
			return OLE_E_BLANK;
		context->IASetInputLayout( p->m_value );
		return S_OK;
	}
};