#include "stdafx.h"
#include "ColorMap.h"
#include "mapHacks.h"
#include <Utils/md4.h>

/*
const char* const EffectImpl<ColorMap>::s_effectName = "Color Map";
static const ApeEffectBase::Metadata s_metadada{ "Color Map", L"colormap.ape", &ApeEffectImpl<ColorMap>::create };
const EffectBase::Metadata& ColorMap::metadata(){ return s_metadada; }
*/
IMPLEMENT_APE_EFFECT( ColorMap, "Color Map", "colormap.ape" );

ColorMap::ColorMap( C_RBASE* pThis ) :
	m_pNative( pThis )
{ }

HRESULT ColorMap::updateParameters( Binder& binder )
{
	const HRESULT hr = m_ps.update( binder, nullptr, nullptr );
	if( S_OK == hr )
		m_ps.dropShader();
	return hr;
}

void ColorMap::bindResources()
{
	bindResource<eStage::Pixel>( m_ps.data().bindMap, m_mapSrv );
}

HRESULT ColorMap::render( bool isBeat, RenderTargets& rt )
{
	const C_RBASE* const pfx = m_pNative;
	const uint8_t idMap = MapHacks::getCurrentMapId( pfx );
	if( !MapHacks::isEnabled( pfx, idMap ) )
		return S_FALSE;

	int fb = 0xCCCCCC, fbOut = 0;
	// Run the native update for beat randomization.
	const int res = m_pNative->render( nullptr, isBeat ? 1 : 0, &fb, &fbOut, 1, 1 );

	const uint32_t* pTable = MapHacks::getTablePointer( pfx, idMap );
	const __m128i newHash = hashBuffer( pTable, 0x400 );
	if( nullptr == m_mapSrv || m_hash != newHash )
	{
		m_hash = newHash;
		CHECK( updateTexture( pTable ) );
	}

	const auto sourceChannel = MapHacks::getSourceChannel( pfx );
	const auto blend = MapHacks::getDestBlendMode( pfx );
	const auto blendValRaw = MapHacks::getAdjustableValue( pfx );
	const float blendVal = (float)( blendValRaw ) * ( 1.0f / 255.0f );
	BoolHr hrUpdate;
	hrUpdate.updateValue( m_ps.data().sourceChannel, (uint32_t)sourceChannel );
	hrUpdate.updateValue( m_ps.data().blend, (uint32_t)blend );
	hrUpdate.updateValue( m_ps.data().blendVal, blendVal );
	if( hrUpdate.value() )
		m_ps.dropShader();

	if( !m_ps.hasShader() )
		CHECK( m_ps.compile( Hlsl::includes(), stateOffset() ) );

	omBlend( eBlend::None );
	BoundPsResource boundFrame;
	CHECK( rt.writeToNext( boundFrame ) );

	setShaders( StaticResources::fullScreenTriangle, nullptr, m_ps.ptr( false ) );
	drawFullscreenTriangle( false );
	return S_OK;
}

HRESULT ColorMap::updateTexture( const uint32_t* pData )
{
	if( m_mapSrv )
	{
		// This happens much less often than once per frame, that's why using D3D11_USAGE_DEFAULT texture with UpdateSubresource.
		// Dynamic resources are only good when they're indeed dynamic.
		context->UpdateSubresource( m_mapTexture, 0, nullptr, pData, 0x400, 0x400 );
		return S_OK;
	}

	constexpr DXGI_FORMAT fmt = DXGI_FORMAT_B8G8R8X8_UNORM;
	CD3D11_TEXTURE1D_DESC texDesc{ fmt, 0x100, 1, 1 };
	D3D11_SUBRESOURCE_DATA initialData{ pData, 0x400, 0x400 };
	CHECK( device->CreateTexture1D( &texDesc, &initialData, &m_mapTexture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE1D, fmt };
	CHECK( device->CreateShaderResourceView( m_mapTexture, &srvDesc, &m_mapSrv ) );

	bindResource<eStage::Pixel>( m_ps.data().bindMap, m_mapSrv );

	return S_OK;
}