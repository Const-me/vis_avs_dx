#include "stdafx.h"
#include "RenderTarget.h"
#include <Interop/interop.h>
#include <Render/Binder.h>

HRESULT RenderTarget::create( const CSize& size, bool rt, bool unorderedAccess )
{
	CD3D11_TEXTURE2D_DESC texDesc{ DXGI_FORMAT_R10G10B10A2_TYPELESS, (UINT)size.cx, (UINT)size.cy, 1, 1, D3D11_BIND_SHADER_RESOURCE };
	if( rt )
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if( unorderedAccess )
		texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &m_tex ) );

	if( rt )
	{
		CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R10G10B10A2_UNORM };
		CHECK( device->CreateRenderTargetView( m_tex, &rtvDesc, &m_rtv ) );
	}

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R10G10B10A2_UNORM };
	CHECK( device->CreateShaderResourceView( m_tex, &srvDesc, &m_srv ) );

	return S_OK;
}

HRESULT RenderTarget::create()
{
	const CSize size = getRenderSize();
	return create( size, true, true );
}

void RenderTarget::destroy()
{
	m_rtv = nullptr;
	m_rtvInteger = nullptr;
	m_srv = nullptr;
	m_tex = nullptr;
	m_uav = nullptr;
}

void RenderTarget::clear( const Vector4& color )
{
	context->ClearRenderTargetView( m_rtv, &color.x );
}

void RenderTarget::clear()
{
	/* #ifdef DEBUG
		clear( Vector4{ 1, 1, 1, 1 } * 0.25f );
	#else
		clear( Vector4::Zero );
	#endif */
	clear( Vector4::Zero );
}

void RenderTarget::clear( const Vector3& color )
{
	Vector4 v4 = color.operator DirectX::XMVECTOR();
	v4.w = 1;
	clear( v4 );
}

ID3D11RenderTargetView* RenderTarget::getIntegerRt()
{
	if( m_rtvInteger )
		return m_rtvInteger;
	static bool failed = false;
	if( failed )
		return m_rtv;

	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R10G10B10A2_UINT };
	const HRESULT hr = device->CreateRenderTargetView( m_tex, &rtvDesc, &m_rtvInteger );
	if( FAILED( hr ) )
	{
		failed = true;
		return m_rtv;
	}
	return m_rtvInteger;
}

void RenderTarget::bindTarget()
{
	ID3D11RenderTargetView* const v = m_rtv;
	context->OMSetRenderTargets( 1, &v, nullptr );
}

void RenderTarget::bindTargetForLogicOp()
{
	ID3D11RenderTargetView* const v = getIntegerRt();
	context->OMSetRenderTargets( 1, &v, nullptr );
}

void RenderTarget::copyTo( const RenderTarget& that ) const
{
	context->CopyResource( that.m_tex, m_tex );
}

HRESULT RenderTarget::createUav()
{
	if( m_uav )
		return S_FALSE;
	if( !m_tex )
		return OLE_E_BLANK;	// Uninitialized object

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_TEXTURE2D, format };
	CHECK( device->CreateUnorderedAccessView( m_tex, &uavDesc, &m_uav ) );
	return S_OK;
}

BoundSrv<eStage::Pixel> RenderTarget::psView() const
{
	assert( nullptr != m_srv );
	return eastl::move( BoundSrv<eStage::Pixel>{ Binder::psPrevFrame, m_srv } );
}