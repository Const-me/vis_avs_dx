#include "stdafx.h"
#include "FrameTexture.h"
#include <Resources/staticResources.h>

HRESULT FrameTexture::haveSample( IMFSample* sample )
{
	DWORD nBuffers;
	CHECK( sample->GetBufferCount( &nBuffers ) );
	if( 0 == nBuffers )
		return S_FALSE;
	for( DWORD i = 0; i < nBuffers; i++ )
	{
		CComPtr<IMFMediaBuffer> buffer;
		CHECK( sample->GetBufferByIndex( i, &buffer ) );

		CComQIPtr<IMFDXGIBuffer> dxgiBuffer = buffer.operator ->();
		if( !dxgiBuffer )
			continue;

		CComPtr<ID3D11Texture2D> texture;
		CHECK( dxgiBuffer->GetResource( IID_PPV_ARGS( &texture ) ) );

		CSLock __lock( m_cs );
		m_texture = texture;
		return S_OK;
	}
	return E_NOINTERFACE;
}

HRESULT FrameTexture::getTexture( CComPtr<ID3D11ShaderResourceView>& srv )
{
	CSLock __lock( m_cs );
	if( !m_texture )
	{
		srv = StaticResources::blackTexture;
		return S_FALSE;
	}

	if( !m_srv )
	{
		D3D11_TEXTURE2D_DESC desc;
		m_texture->GetDesc( &desc );
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		CComPtr<ID3D11Texture2D> viewTexture;
		CHECK( device->CreateTexture2D( &desc, nullptr, &viewTexture ) );

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format };
		CHECK( device->CreateShaderResourceView( viewTexture, &srvDesc, &m_srv ) );
		m_viewTexture = viewTexture;
	}

	context->CopyResource( m_viewTexture, m_texture );
	srv = m_srv;
	return S_OK;
}

void FrameTexture::destroy()
{
	CSLock __lock( m_cs );
	m_texture = nullptr;
	m_viewTexture = nullptr;
	m_srv = nullptr;
}