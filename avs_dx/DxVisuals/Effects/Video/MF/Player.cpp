#include "stdafx.h"
#include "Player.h"
#include "mfStatic.h"
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

	return E_NOTIMPL;
}

HRESULT Player::stop()
{
	if( !m_sink )
		return S_FALSE;
	m_sink->Shutdown();
	m_sink = nullptr;
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
		CComPtr<IMFTopologyNode> node;
		CHECK( MFCreateTopologyNode( MF_TOPOLOGY_SOURCESTREAM_NODE, &node ) );
		CHECK( node->SetUnknown( MF_TOPONODE_SOURCE, source ) );
		CHECK( node->SetUnknown( MF_TOPONODE_PRESENTATION_DESCRIPTOR, pd ) );
		CHECK( node->SetUnknown( MF_TOPONODE_STREAM_DESCRIPTOR, sd ) );
		CHECK( topology->AddNode( node ) );

		// Create the stream sink
		CComPtr<IMFStreamSink> streamSink;
		CHECK( MediaSink::create( m_sink, m_texture, streamSink ) );

		// Add an output node to the topology
		node = nullptr;
		CHECK( MFCreateTopologyNode( MF_TOPOLOGY_OUTPUT_NODE, &node ) );
		CHECK( node->SetObject( streamSink ) );
		CHECK( node->SetUINT32( MF_TOPONODE_STREAMID, 0 ) );
		// CHECK( node->SetUINT32( MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE ) );
		CHECK( topology->AddNode( node ) );
	}
	return S_OK;
}