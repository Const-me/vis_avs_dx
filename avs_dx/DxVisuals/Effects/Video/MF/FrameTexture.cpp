#include "stdafx.h"
#include "FrameTexture.h"
#include <Resources/staticResources.h>

HRESULT FrameTexture::create( const CSize& size )
{
	if( m_srv && m_rect.Size() == size )
		return S_FALSE;

	m_texture = nullptr;
	m_srv = nullptr;

	CD3D11_TEXTURE2D_DESC desc{ DXGI_FORMAT_R10G10B10A2_UNORM, (UINT)size.cx, (UINT)size.cy, 1, 1 };
	CHECK( device->CreateTexture2D( &desc, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format };
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srv ) );

	m_rect = CRect( CPoint( 0, 0 ), size );
	return S_OK;
}

HRESULT FrameTexture::getView( CComPtr<ID3D11ShaderResourceView>& srv )
{
	if( !m_texture )
	{
		srv = StaticResources::blackTexture;
		return S_FALSE;
	}
	srv = m_srv;
	return S_OK;
}

void FrameTexture::destroy()
{
	m_texture = nullptr;
	m_srv = nullptr;
}