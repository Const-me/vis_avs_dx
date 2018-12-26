#include "stdafx.h"
#include <versionhelpers.h>
#include "Player.h"
#include "mfStatic.h"
#include <Resources/staticResources.h>

HRESULT createPlayer( CComPtr<iPlayer>& player )
{
	// Technically possible to support Win7 by implementing a custom IMFMediaSink and using it with a session and topology, but it's hard to do, and extremely hard to make it work with real-life video files.
	// I've tried it first.
	// See e.g. https://stackoverflow.com/q/52198300/126995
	// BTW, on my Win10, 32-bit version of MFMediaEngine.dll is 4.12MB, that's more than D3D runtime, d3d11.dll is only 2.3MB.
	if( !IsWindows8OrGreater() )
	{
		logError( "Video rendering is only supported on Windows 8.0 or newer." );
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
	// Can't do nothing on this thread, it has wrong COM apartment type. Apparently, IMFMediaEngine requires MTA.
	return S_OK;
}

HRESULT Player::close()
{
	CSLock __lock( m_cs );
	m_path.Empty();
	m_updated = true;
	return S_OK;
}

HRESULT Player::render()
{
	bool updated;
	bool haveVideo;
	CComBSTR path;
	{
		CSLock __lock( m_cs );
		haveVideo = m_path;
		updated = m_updated;
		if( updated )
			path = m_path;	// Don't copy BSTR strings each frame, only when updated
		m_updated = false;
	}

	if( updated )
	{
		if( haveVideo )
		{
			CHECK( ensureEngine() );
			CHECK( m_engine->SetSource( path ) );
			CHECK( m_engine->Play() );
		}
		else
		{
			if( m_engine )
				CHECK( m_engine->Pause() );
		}
	}

	if( !haveVideo )
		return S_FALSE;

	LONGLONG pts;
	if( m_engine->OnVideoStreamTick( &pts ) == S_OK )
	{
		if( !m_texture )
			CHECK( m_texture.create() );
		const MFARGB border = { 0, 0, 0, 0 };
		const HRESULT hr = m_engine->TransferVideoFrame( m_texture.texture(), nullptr, m_texture.rectangle(), &border );
		CHECK( hr );
	}

	return m_texture ? S_OK : S_FALSE;
}

HRESULT Player::ensureEngine()
{
	if( m_engine )
		return S_FALSE;

	// Initialize global stuff
	CHECK( coInit() );
	CHECK( mfStartup() );
	CComPtr<IMFMediaEngineClassFactory> factory;
	CHECK( mfEngineFactory( factory ) );

	// Create attributes
	CComPtr<IMFAttributes> a;
	CHECK( MFCreateAttributes( &a, 4 ) );
	IMFMediaEngineNotify* men = this;
	CHECK( a->SetUnknown( MF_MEDIA_ENGINE_CALLBACK, men ) );
	CHECK( a->SetUINT32( MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, FrameTexture::videoFormat ) );
	if( dxgiDeviceManager )
		CHECK( a->SetUnknown( MF_MEDIA_ENGINE_DXGI_MANAGER, dxgiDeviceManager ) );

	// Create the engine
	constexpr DWORD flags = MF_MEDIA_ENGINE_FORCEMUTE;
	CHECK( factory->CreateInstance( flags, a, &m_engine ) );

	// Setup the engine
	CHECK( m_engine->SetMuted( TRUE ) );
	CHECK( m_engine->SetLoop( TRUE ) );
	CHECK( m_engine->SetAutoPlay( TRUE ) );

	return S_OK;
}

HRESULT __stdcall Player::EventNotify( DWORD e, DWORD_PTR param1, DWORD param2 )
{
	// Documentation says MF_MEDIA_ENGINE_CALLBACK attribute, and therefore IMFMediaEngineNotify interface, is mandatory, but we don't have anything to do here.
	return S_OK;
}