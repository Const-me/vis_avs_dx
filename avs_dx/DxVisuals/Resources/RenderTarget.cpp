#include "stdafx.h"
#include "RenderTarget.h"

constexpr DXGI_FORMAT rtFormat = DXGI_FORMAT_R10G10B10A2_UNORM;

HRESULT RenderTarget::create( const CSize& size )
{
	CD3D11_TEXTURE2D_DESC texDesc{ rtFormat, (UINT)size.cx, (UINT)size.cy, 1, 0, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET };
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &m_tex ) );

	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, rtFormat };
	CHECK( device->CreateRenderTargetView( m_tex, &rtvDesc, &m_rtv ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, rtFormat };
	CHECK( device->CreateShaderResourceView( m_tex, &srvDesc, &m_srv ) );

	return S_OK;
}

void RenderTarget::destroy()
{
	m_rtv = nullptr;
	m_srv = nullptr;
	m_tex = nullptr;
}

void RenderTarget::clear( const Vector4& color )
{
	context->ClearRenderTargetView( m_rtv, &color.x );
}

void RenderTarget::bindTarget()
{
	ID3D11RenderTargetView* v = m_rtv;
	context->OMSetRenderTargets( 1, &v, nullptr );
}

void RenderTarget::bindView( UINT slot ) const
{
	bindResource<eStage::Pixel>( slot, m_srv );
}

void RenderTarget::copyTo( const RenderTarget& that ) const
{
	context->CopyResource( that.m_tex, m_tex );
}

void RenderTarget::swap( RenderTarget& rt )
{
	std::swap( m_tex, rt.m_tex );
	std::swap( m_rtv, rt.m_rtv );
	std::swap( m_srv, rt.m_srv );
}