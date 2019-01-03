#include "stdafx.h"
#include "MosaicTexture.h"
#include <../InteropLib/interop.h>

void MosaicTexture::destroy()
{
	m_texture = nullptr;
	m_srv = nullptr;
}

void MosaicTexture::onRenderSizeChanged()
{
	destroy();
}

HRESULT MosaicTexture::createTextures()
{
	if( m_srv )
		return S_FALSE;

	destroy();

	const CSize size = getRenderSize();
	constexpr DXGI_FORMAT format = RenderTarget::format;

	CD3D11_TEXTURE2D_DESC texDesc{ format, (UINT)size.cx, (UINT)size.cy, 1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET };
	texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.MipLevels = 0;
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &m_texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, format };
	CHECK( device->CreateShaderResourceView( m_texture, &srvDesc, &m_srv ) );

	return S_OK;
}

HRESULT MosaicTexture::update( const RenderTarget& src )
{
	CHECK( createTextures() );

	context->CopySubresourceRegion( m_texture, 0, 0, 0, 0, src.texture(), 0, nullptr );
	context->GenerateMips( m_srv );

	return S_OK;
}