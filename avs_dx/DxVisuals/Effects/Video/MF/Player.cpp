#include "stdafx.h"
#include "Player.h"
#include "mfStatic.h"
#include "PropVariant.hpp"
#pragma comment( lib, "Mf.lib" )

HRESULT Player::start( LPCTSTR pathToVideo )
{
	m_session = nullptr;
	m_sink = nullptr;
	m_texture.destroy();

	if( !PathFileExists( pathToVideo ) )
		return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

	CHECK( mfStartup() );

	// Create the media session.
	CHECK( MFCreateMediaSession( nullptr, &m_session ) );

	// Start pulling events from the media session
	CHECK( startListen( m_session ) );

	CComPtr<IMFSourceResolver> sourceResolver;
	CHECK( mfSourceResolver( sourceResolver ) );

	// Create the media source
	CComPtr<IMFMediaSource> source;
	{
		MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
		CComPtr<IUnknown> unkSource;
		CHECK( sourceResolver->CreateObjectFromURL( pathToVideo, MF_RESOLUTION_MEDIASOURCE, nullptr, &objectType, &unkSource ) );
		CHECK( unkSource->QueryInterface( IID_PPV_ARGS( &source ) ) );
	}

	// Create the presentation descriptor for the media source.
	CComPtr<IMFPresentationDescriptor> pd;
	CHECK( source->CreatePresentationDescriptor( &pd ) );

	// Create a partial topology
	CComPtr<IMFTopology> topology;
	CHECK( createPlaybackTopology( source, pd, topology ) );

	CHECK( m_session->SetTopology( 0, topology ) );

	PropVariant startTime{ 0 };
	CHECK( m_session->Start( &GUID_NULL, startTime ) );

	if( dbgLogStuff )
		logDebug( "Player::start completed successfully" );
	return S_OK;
}

HRESULT Player::createPlaybackTopology( IMFMediaSource* source, IMFPresentationDescriptor *pd, CComPtr<IMFTopology>& topology )
{
	// Create a new topology
	CHECK( MFCreateTopology( &topology ) );

	// Get the number of streams in the media source
	DWORD cSourceStreams = 0;
	CHECK( pd->GetStreamDescriptorCount( &cSourceStreams ) );

	bool selectedStream = false;
	for( DWORD i = 0; i < cSourceStreams; i++ )
	{
		if( selectedStream )
		{
			CHECK( pd->DeselectStream( i ) );
			continue;
		}

		BOOL selected;
		CComPtr<IMFStreamDescriptor> sd;
		CHECK( pd->GetStreamDescriptorByIndex( i, &selected, &sd ) );

		CComPtr<IMFMediaTypeHandler> mth;
		CHECK( sd->GetMediaTypeHandler( &mth ) );

		GUID majorType = GUID_NULL;
		CHECK( mth->GetMajorType( &majorType ) );

		if( majorType != MFMediaType_Video )
		{
			CHECK( pd->DeselectStream( i ) );
			continue;
		}

		// Found the first video stream. Use it.
		selectedStream = true;
		CHECK( pd->SelectStream( i ) );

		// Add a source node to a topology
		CComPtr<IMFTopologyNode> src;
		CHECK( MFCreateTopologyNode( MF_TOPOLOGY_SOURCESTREAM_NODE, &src ) );
		CHECK( src->SetUnknown( MF_TOPONODE_SOURCE, source ) );
		CHECK( src->SetUnknown( MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd ) );
		CHECK( src->SetUnknown( MF_TOPONODE_STREAM_DESCRIPTOR, sd ) );
		CHECK( topology->AddNode( src ) );

		// Create the stream sink
		CComPtr<IMFStreamSink> streamSink;
		CHECK( MediaSink::create( m_sink, m_texture, streamSink ) );

		// Add an output node to the topology
		CComPtr<IMFTopologyNode> dest;
		CHECK( MFCreateTopologyNode( MF_TOPOLOGY_OUTPUT_NODE, &dest ) );
		CHECK( dest->SetObject( streamSink ) );
		CHECK( dest->SetUINT32( MF_TOPONODE_STREAMID, 0 ) );
		// CHECK( node->SetUINT32( MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE ) );
		CHECK( topology->AddNode( dest ) );

		// Connect the source node to the output node
		CHECK( src->ConnectOutput( 0, dest, 0 ) );
	}
	return S_OK;
}

HRESULT Player::onEvent( MediaEventType eventType, HRESULT hrStatus )
{
	dbgLogMediaEvent( "Player::onEvent", eventType, hrStatus );

	switch( eventType )
	{
	case MESessionStarted:
		return m_sink->requestSample();

	case MESessionClosed:
		m_evtClosed.set();
		return S_OK;

	case MEEndOfPresentation:
		CHECK( m_session->Stop() );
		return S_OK;

	case MESessionStopped:
		PropVariant startTime{ 0 };
		CHECK( m_session->Start( &GUID_NULL, startTime ) );
		return S_OK;
	}
	return S_FALSE;
}

HRESULT Player::stop()
{
	if( m_session )
	{
		CHECK( m_session->Close() );
		CHECK( m_evtClosed.wait() );
		CHECK( stopListen() );
	}
	if( m_sink )
		CHECK( m_sink->Shutdown() );

	m_session = nullptr;
	m_sink = nullptr;
	m_texture.destroy();
	return S_OK;
}

HRESULT createPlayer( LPCTSTR pathToVideo, CComPtr<iPlayer>& player )
{
	CComPtr<CComObject<Player>> result;
	CHECK( createInstance( result ) );
	CHECK( result->start( pathToVideo ) );

	player = result.operator ->();
	return S_OK;
}