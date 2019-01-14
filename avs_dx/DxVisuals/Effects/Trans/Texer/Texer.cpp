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

	omBlend( eBlend::Premultiplied );
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->DrawInstancedIndirect( stateBuffer(), stateOffset() * 4 );
	return S_OK;
}