#include "stdafx.h"
#include "PictureEffect.h"
#include <Utils/DXTK/WICTextureLoader.h>
#include <Utils/findAssets.h>

static bool isPictureExt( const CString& ext )
{
	return ext == L".jpg" || ext == L".jpeg" || ext == L".png" || ext == L".dds" || ext == L".bmp" || ext == L".gif";
}

// Search for suitable video files under g_path, "C:\Soft\Winamp\Plugins\avs" on my PC, add them to the combobox.
int initPictureCombobox( HWND wndDialog, HWND wndComboBox, char *selectedName )
{
	return initAssetsCombobox( wndDialog, wndComboBox, selectedName, &isPictureExt );
}

HRESULT pictureOpen( iRootEffect* pEffect, const char *selection )
{
	PictureEffect* const pfx = dynamic_cast<PictureEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->open( selection );
}

HRESULT pictureClose( iRootEffect* pEffect )
{
	PictureEffect* const pfx = dynamic_cast<PictureEffect*>( pEffect );
	if( nullptr == pfx )
		return E_INVALIDARG;
	return pfx->close();
}

IMPLEMENT_EFFECT( PictureEffect, C_PictureClass );

HRESULT PictureEffect::close()
{
	CSLock __lock( m_cs );
	m_path = "";
	m_updated = true;
	return S_OK;
}

HRESULT PictureEffect::open( const char* selection )
{
	CSLock __lock( m_cs );
	CHECK( getAssetFilePath( selection, &isPictureExt, m_path ) );
	m_updated = true;
	return S_OK;
}

eBlendMode PictureStructs::AvsState::blendMode() const
{
	if( blend )
		return eBlendMode::Additive;
	if( blendavg )
		return eBlendMode::Fifty;
	if( adapt )
		return eBlendMode::Adjustable;
	return eBlendMode::Replace;
}

HRESULT PictureEffect::render( bool isBeat, RenderTargets& rt )
{
	if( !avs->enabled )
		return S_FALSE;

	bool updated;
	bool havePicture;
	CString path;
	{
		CSLock lk( m_cs );
		havePicture = m_path.GetLength() > 0;
		updated = m_updated;
		if( updated )
			path = m_path;
		m_updated = false;
	}

	if( updated )
	{
		if( havePicture )
		{
			CHECK( coInit() );
			m_pic = nullptr;
			CComPtr<ID3D11Resource> res;
			CHECK( DirectX::CreateWICTextureFromFile( device, context, path, &res, &m_pic ) );
		}
		else
		{
			m_pic = nullptr;
		}
	}

	if( !m_pic )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	CHECK( rt.writeToLast( false ) );
	BIND_PS_SRV( 15, m_pic );
	const auto mode = avs->blendMode();

	float factor = 0.5;
	switch( mode )
	{
	case eBlendMode::Replace:
		omBlend( eBlend::Premultiplied );
		break;
	case eBlendMode::Additive:
		omBlend( eBlend::Add );
		break;
	case eBlendMode::Adjustable:
		m_fade.update( avs->persist );
		factor += 0.5f * m_fade.value( isBeat );
	case eBlendMode::Fifty:
		omCustomBlend( factor );
		break;
	default:
		return E_INVALIDARG;
	}

	drawFullscreenTriangle( false );
	return S_OK;
}