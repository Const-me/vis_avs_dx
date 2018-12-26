#include "stdafx.h"
#include "VideoEffect.h"
#include "interop.h"

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

HRESULT VideoStructs::VsData::updateAvs( const AvsState& ass )
{
	return S_FALSE;
}

HRESULT VideoEffect::render( bool isBeat, RenderTargets& rt )
{
	if( !m_player )
		return S_FALSE;

	CComPtr<ID3D11ShaderResourceView> srv;
	CHECK( m_player->getTexture( srv ) );

	CHECK( renderer.bindShaders( isBeat ) );

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );

	return S_OK;
}