#include "stdafx.h"
#include "FrameTexture.h"
#include <Resources/staticResources.h>

CSize getRenderSize();

HRESULT FrameTexture::create()
{
	m_texture = nullptr;
	m_srv = nullptr;

	const CSize size = getRenderSize();
	CD3D11_TEXTURE2D_DESC desc{ videoFormat, (UINT)size.cx, (UINT)size.cy, 1, 1 };

	// Fix the followinug error:
	// D3D11 ERROR: ID3D11DeviceContext::CreateVideoProcessorOutputView: Resource must have bind flag D3D11_BIND_RENDER_TARGET - was created with 8! [ STATE_CREATION ERROR #3145933: CREATEVIDEOPROCESSOROUTPUTVIEW_INVALIDBIND]
	desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

	CHECK( device->CreateTexture2D( &desc, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, videoFormat };
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srv ) );

	m_rect = CRect( CPoint( 0, 0 ), size );
	return S_OK;
}

void FrameTexture::destroy()
{
	m_texture = nullptr;
	m_srv = nullptr;
}