#include "stdafx.h"
#include <versionhelpers.h>
#include "Player.h"
#include "mfStatic.h"
#include <Resources/staticResources.h>

HRESULT createPlayer( CComPtr<iPlayer>& player )
{
	if( !IsWindows8OrGreater() )
	{
		logError( "Video rendering is only supported on Windows 8.0 or newer" );
		return HRESULT_FROM_WIN32( ERROR_OLD_WIN_VERSION );
	}

	CComPtr<CComObject<Player>> result;
	CHECK( createInstance( result ) );
	player = result.operator ->();
	return S_OK;
}

HRESULT Player::open( LPCTSTR pathToVideo )
{
	CSLock __lock( m_cs );
	m_path = pathToVideo;
	m_updated = true;
	// Can't do nothing on this thread, it has wrong COM apartment type.
	return S_OK;
}

HRESULT Player::close()
{
	CSLock __lock( m_cs );
	m_path.Empty();
	m_updated = true;
	return S_OK;
}

HRESULT Player::getTexture( CComPtr<ID3D11ShaderResourceView>& srv )
{
	bool updated;
	CComBSTR path;;
	{
		CSLock __lock( m_cs );
		path = m_path;
		updated = m_updated;
		m_updated = false;
	}

	if( updated )
	{
		if( path )
		{
			CHECK( ensureEngine() );
			CHECK( m_engine->SetSource( path ) );
			CHECK( m_engine->Play() );
		}
		else
		{
			if( m_engine )
				CHECK( m_engine->Pause() );
			m_texture.destroy();
		}
	}

	if( !path )
	{
		srv = StaticResources::blackTexture;
		return S_FALSE;
	}

	LONGLONG pts;
	if( m_engine->OnVideoStreamTick( &pts ) == S_OK )
	{
		if( !m_texture )
		{
			CSize sz;
			CHECK( m_engine->GetNativeVideoSize( (DWORD*)&sz.cx, (DWORD*)&sz.cy ) );
			CHECK( m_texture.create( sz ) );
		}
		const MFARGB border = { 0, 0, 0, 0 };
		const HRESULT hr = m_engine->TransferVideoFrame( m_texture.texture(), nullptr, m_texture.rectangle(), &border );
		CHECK( hr );
	}

	return m_texture.getView( srv );
}

HRESULT Player::ensureEngine()
{
	if( m_engine )
		return S_FALSE;

	CHECK( coInit() );
	CHECK( mfStartup() );
	CComPtr<IMFMediaEngineClassFactory> factory;
	CHECK( mfEngineFactory( factory ) );


	CComPtr<IMFAttributes> a;
	CHECK( MFCreateAttributes( &a, 4 ) );

	IMFMediaEngineNotify* men = this;
	CHECK( a->SetUnknown( MF_MEDIA_ENGINE_CALLBACK, men ) );
	CHECK( a->SetUINT32( MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, FrameTexture::videoFormat ) );

	if( dxgiDeviceManager )
		CHECK( a->SetUnknown( MF_MEDIA_ENGINE_DXGI_MANAGER, dxgiDeviceManager ) );

	constexpr DWORD flags = MF_MEDIA_ENGINE_FORCEMUTE;

	CHECK( factory->CreateInstance( flags, a, &m_engine ) );

	CHECK( m_engine->SetMuted( TRUE ) );
	CHECK( m_engine->SetLoop( TRUE ) );
	CHECK( m_engine->SetAutoPlay( TRUE ) );

	return S_OK;
}

HRESULT __stdcall Player::EventNotify( DWORD e, DWORD_PTR param1, DWORD param2 )
{
	return S_OK;
}