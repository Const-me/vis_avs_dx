#include "stdafx.h"
#include "VideoEffect.h"
#include "interop.h"
#include <Utils/findAssets.h>

static bool isVideoFileExtension( const CString& ext )
{
	return ext == L".mp4" || ext == L".mpeg" || ext == L".mkv" || ext == L".avi" || ext == L".wmv";
}

int initVideoCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName )
{
	return initAssetsCombobox( wndDialog, wndComboBox, selectedName, &isVideoFileExtension );
}

HRESULT getVideoFilePath( const char *selection, CString& result )
{
	return getAssetFilePath( selection, &isVideoFileExtension, result );
}

IMPLEMENT_EFFECT( VideoEffect, C_AVIClass );

// Start playing the video
HRESULT videoOpen( iRootEffect* pEffect, const char *selection )
{
	VideoEffect* const pfx = dynamic_cast<VideoEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->open( selection );
}

// Close the video
HRESULT videoClose( iRootEffect* pEffect )
{
	VideoEffect* const pfx = dynamic_cast<VideoEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->close();
}

// Get full path from the combobox selection
HRESULT getVideoFilePath( const char *selection, CString& result );

HRESULT VideoEffect::open( const char* selection )
{
	CString target;
	const HRESULT hr = getVideoFilePath( selection, target );
	if( FAILED( hr ) )
	{
		close();
		return hr;
	}

	if( !m_player )
		CHECK( createPlayer( m_player ) );

	CHECK( m_player->open( target ) );
	return S_OK;
}

HRESULT VideoEffect::close()
{
	if( !m_player )
		return S_FALSE;
	CHECK( m_player->close() );
	return S_OK;
}

eBlendMode VideoEffect::AvsState::blendMode() const
{
	if( blend )
		return eBlendMode::Additive;
	if( blendavg )
		return eBlendMode::Fifty;
	if( adapt )
		return eBlendMode::Adjustable;
	return eBlendMode::Replace;
}

VideoEffect::VideoEffect( AvsState *pState ) : avs( *pState )
{
	if( avs.persist <= 0 )
		avs.persist = 1;
}

VideoEffect::~VideoEffect()
{
	logShutdown( "~VideoEffect" );
	if( m_player )
	{
		m_player->shutdown();
		m_player = nullptr;
	}
}

HRESULT VideoEffect::render( bool isBeat, RenderTargets& rt )
{
	if( !m_player || !avs.enabled )
		return S_FALSE;

	const HRESULT hr = m_player->render();
	CHECK( hr );
	if( hr == S_FALSE )
		return S_FALSE;

	const auto mode = avs.blendMode();
	if( mode == eBlendMode::Replace )
	{
		RenderTarget& target = rt.lastWritten();
		if( !target )
			CHECK( target.create() );
		context->CopyResource( target.texture(), m_player->texture() );
		return S_OK;
	}

	// Blender class would work here but is slower, it compiles everything into the shader, with beat-adjustable blending would waste too much time recompiling.
	// Fortunately, D3D offers enough blending customizations for the OM stage.

	rt.writeToLast( false );

	float factor = 0.5;
	switch( mode )
	{
	case eBlendMode::Additive:
		omBlend( eBlend::Add );
		break;
	case eBlendMode::Adjustable:
		m_fade.update( avs.persist );
		factor += 0.5f * m_fade.value( isBeat );
	case eBlendMode::Fifty:
		omCustomBlend( factor );
		break;
	default:
		return E_INVALIDARG;
	}
	setShaders( StaticResources::fullScreenTriangle, nullptr, StaticResources::copyTexture );
	bindResource<eStage::Pixel>( 127, m_player->view() );
	drawFullscreenTriangle( false );
	bindResource<eStage::Pixel>( 127 );
	return S_OK;
}