#include "stdafx.h"
#include "MipMaps.h"
#include <Interop/interop.h>
#include <Render/Binder.h>

void MipMaps::destroy()
{
	m_texture = nullptr;
	m_srv = nullptr;
}

void MipMaps::onRenderSizeChanged()
{
	destroy();
}

HRESULT MipMaps::createTextures()
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

HRESULT MipMaps::update( const RenderTarget& src )
{
	CHECK( createTextures() );

	context->CopySubresourceRegion( m_texture, 0, 0, 0, 0, src.texture(), 0, nullptr );
	context->GenerateMips( m_srv );
	return S_OK;
}

MipMapsRenderer::MipMapsRenderer( const char* profileName ) :
	m_profileMipmaps( profileName ) { }

HRESULT MipMapsRenderer::updateMipmaps( RenderTarget& src )
{
	CHECK( m_mipMaps.update( src ) );
	m_profileMipmaps.mark();
	return S_OK;
}

HRESULT MipMapsRenderer::bindMipmaps( UINT samplerSlot )
{
	bindResource<eStage::Pixel>( Binder::psPrevFrame, m_mipMaps.srv() );

#ifdef DEBUG
	if( !m_sampler )
	{
		CD3D11_SAMPLER_DESC sd{ D3D11_DEFAULT };
		CHECK( device->CreateSamplerState( &sd, &m_sampler ) );
		bindSampler<eStage::Pixel>( samplerSlot, m_sampler );
	}
#else
	// Default sampler state is what we need here
	bindSampler<eStage::Pixel>( samplerSlot );
#endif
	return S_OK;
}