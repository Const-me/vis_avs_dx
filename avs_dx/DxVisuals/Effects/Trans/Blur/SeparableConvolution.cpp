#include "stdafx.h"
#include "SeparableConvolution.h"
#include <Resources/RenderTarget.h>
#include "warpSize.h"
#include <Utils/PerfMeasure.h>

void SeparableConvolution::destroy()
{
	m_uav = nullptr;
	m_srv = nullptr;
}

CSize getRenderSize();

HRESULT SeparableConvolution::ensureBuffer()
{
	if( m_uav )
		return S_FALSE;

	const CSize renderSize = getRenderSize();
	const CSize size{ renderSize.cy, renderSize.cx };
	constexpr DXGI_FORMAT format = RenderTarget::format;

	CComPtr<ID3D11Texture2D> texture;
	CD3D11_TEXTURE2D_DESC texDesc{ format, (UINT)size.cx, (UINT)size.cy, 1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	CHECK( device->CreateTexture2D( &texDesc, nullptr, &texture ) );

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ D3D11_SRV_DIMENSION_TEXTURE2D, format };
	CHECK( device->CreateShaderResourceView( texture, &srvDesc, &m_srv ) );

	CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{ D3D11_UAV_DIMENSION_TEXTURE2D, format };
	CHECK( device->CreateUnorderedAccessView( texture, &uavDesc, &m_uav ) );

	return S_OK;
}

HRESULT SeparableConvolution::PassShaderData::updateAvs( const ShaderParams& params )
{
	BoolHr res;
	res.updateValue( size, params.inputSize );
	res.updateValue( WARP_SIZE, getWarpSize() );
	if( params.pKernel != m_pKernel )
	{
		m_pKernel = params.pKernel;
		res.combine( true );
	}

	if( res.failed() || !res.value() )
		return res;

	m_groups.cy = params.inputSize.cy;

	const uint32_t groupSize = WARP_SIZE * warpsPerGroup;
	const uint32_t kernelRadius = m_pKernel->radius;
	const uint32_t writesPerGroup = groupSize + 2 - kernelRadius - kernelRadius;
	const uint32_t x = params.inputSize.cx - params.pKernel->radius + 1;
	m_groups.cx = ( x + writesPerGroup - 1 ) / writesPerGroup;
	return S_OK;
}

HRESULT SeparableConvolution::PassShaderData::defines( Hlsl::Defines& def ) const
{
	CHECK( __super::defines( def ) );
	def.set( "kernelRadius", m_pKernel->radius );
	def.set( "KERNEL_VALUES", m_pKernel->values );
	return S_OK;
}

void SeparableConvolution::PassShaderData::dispatch() const
{
	context->Dispatch( m_groups.cx, m_groups.cy, 1 );
}

HRESULT SeparableConvolution::updateShader( PassShader& s, Binder& binder, const ShaderParams& params )
{
	BoolHr res = s.update( binder, params, nullptr );
	if( res.failed() )
		return res;
	if( !res.value() )
		return res;
	res.combine( s.compile( Hlsl::includes(), 0 ) );
	return res;
}

HRESULT SeparableConvolution::update( Binder& binder, const ConvolutionKernel *pKernel )
{
	const CSize size = getRenderSize();
	ShaderParams sp{ size, pKernel };
	BoolHr res = updateShader( m_shaders[ 0 ], binder, sp );

	sp.inputSize = CSize{ size.cy, size.cx };
	res.combine( updateShader( m_shaders[ 1 ], binder, sp ) );
	return res;
}

HRESULT SeparableConvolution::run( RenderTargets& rt )
{
	CHECK( ensureBuffer() );
	omClearTargets();

	// PerfMeasure( "SeparableConvolution::run" );

	RenderTarget& target = rt.lastWritten();
	if( !target )
		return S_FALSE;	// Nothing to blur, there's no prev.frame texture.

	// First blur pass, along X: read from the last written RT texture, write to m_uav
	{
		if( !m_shaders[ 0 ].bind( false ) )
			return S_FALSE;
		const auto& cs = m_shaders[ 0 ].data();
		auto boundSrv = boundResource<eStage::Compute>( cs.bindSource, target.srv() );
		auto boundUav = boundResource( cs.bindDest, m_uav );
		cs.dispatch();
	}
	
	// Second blur pas, along Y: read from m_srv, write to one of the m_outputViews
	{
		if( !m_shaders[ 1 ].bind( false ) )
			return S_FALSE;

		CHECK( rt.lastWritten().createUav() );

		const auto& cs = m_shaders[ 1 ].data();
		auto boundSrv = boundResource<eStage::Compute>( cs.bindSource, m_srv );
		auto boundUav = boundResource( cs.bindDest, rt.lastWritten().uav() );
		cs.dispatch();
	}

	return S_OK;
}