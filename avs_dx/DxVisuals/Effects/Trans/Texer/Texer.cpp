#include "stdafx.h"
#include "Texer.h"
#include "texerHacks.h"

IMPLEMENT_APE_EFFECT( Texer, nullptr, "texer.ape" );

Texer::Texer( C_RBASE* pThis ) :
	EffectBase1( pThis )
{ }

HRESULT TexerStructs::StateData::update( const C_RBASE & nativeFx )
{
	const sTexerData data = getTexerData( &nativeFx );
	BoolHr hr = m_sprite.update( data.texture );
	hr.updateValue( m_nParticles, data.particles );
	return hr;
}

HRESULT TexerStructs::VsData::updateDx( const StateData& sd )
{
	BoolHr hr = __super::updateDx( sd );
	hr.updateValue( vertsToDraw, sd.m_nParticles );
	return hr;
}

HRESULT TexerStructs::GsData::updateDx( const StateData& sd )
{
	BoolHr hr = __super::updateDx( sd );
	hr.updateValue( sizeInPixels, sd.m_sprite.size() );
	return hr;
}

inline UINT dispatchMaxima( int x )
{
	return (UINT)( ( x + 13 ) / 14 );
}

HRESULT Texer::render( bool isBeat, RenderTargets& rt )
{
	CHECK( m_tmpBuffer.create() );
	if( !stateData.m_sprite.srv() )
		return S_FALSE;

	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	if( !m_blend )
	{
		CD3D11_BLEND_DESC blendDesc{ D3D11_DEFAULT };
		D3D11_RENDER_TARGET_BLEND_DESC& rt = blendDesc.RenderTarget[ 0 ];
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D11_BLEND_ONE;	// Already premultiplied
		rt.BlendOp = D3D11_BLEND_OP_MAX;
		rt.DestBlend = D3D11_BLEND_ONE;
		rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
		CHECK( device->CreateBlendState( &blendDesc, &m_blend ) );
	}

	{
		ID3D11ShaderResourceView* input = rt.lastWritten().srv();
		if( nullptr == input )
			return S_FALSE;

		omClearTargets();
		BoundSrv<eStage::Compute> boundInput{ 3, input };
		BoundUav tmpTexture{ renderer.compute().bindOutput, m_tmpBuffer.uav() };
		const CSize size = getRenderSize();
		context->Dispatch( dispatchMaxima( size.cx ), dispatchMaxima( size.cy ), 1 );
	}

	// Copy the counter from append-consume buffer to the state buffer
	context->CopyStructureCount( stateBuffer(), stateOffset() * 4, m_tmpBuffer.uav() );

	// Draw the sprites using DrawInstancedIndirect call
	BoundPsResource boundSprite{ 3, stateData.m_sprite.srv() };
	BoundSrv<eStage::Vertex> boundSourceBuffer{ renderer.vertex().bindInput, m_tmpBuffer.srv() };

	rt.advance();
	CHECK( rt.writeToLast( true ) );

	context->OMSetBlendState( m_blend, nullptr, UINT_MAX );
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->DrawInstancedIndirect( stateBuffer(), stateOffset() * 4 );
	return S_OK;
}