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

HRESULT Texer::updateParameters( Binder& binder )
{
	BoolHr hr = __super::updateParameters( binder );

	bool changed = false;
	if( binder.reserveInputSlot( m_maxima.data().bindOutput, eStage::Compute, 'u' ) )
		changed = true;

	m_produce.data().bindOutput = m_maxima.data().bindOutput;

	const CSize currentSize = getRenderSize();
	if( prevSize != currentSize )
	{
		prevSize = currentSize;
		m_produce.data().sizeImage = currentSize;
		m_maxima.data().sizeImage = currentSize;
		changed = true;
	}

	if( changed )
	{
		m_maxima.setUpdated();
		m_produce.setUpdated();
		CHECK( m_maxima.compile( 0 ) );
		CHECK( m_produce.compile( 0 ) );
		hr.combine( true );
	}
	return hr;
}

inline UINT dispatchMaxima( int x )
{
	return (UINT)( ( x + 13 ) / 14 );
}

inline UINT dispatchProduce( int x )
{
	return (UINT)( ( x + 7 ) / 8 );
}

HRESULT Texer::render( bool isBeat, RenderTargets& rt )
{
	CHECK( m_tmpTexture.update() );
	CHECK( m_tmpBuffer.create() );
	if( !stateData.m_sprite.srv() )
		return S_FALSE;

	const CSize size = m_tmpTexture.size();

	// First pass, find local maxima
	if( !m_maxima.bind() )
		return S_FALSE;
	{
		ID3D11ShaderResourceView* input = rt.lastWritten().srv();
		if( nullptr == input )
			return S_FALSE;

		omClearTargets();
		BoundSrv<eStage::Compute> boundInput{ 3, input };
		BoundUav tmpTexture{ m_maxima.data().bindOutput, m_tmpTexture.uav() };
		context->Dispatch( dispatchMaxima( size.cx ), dispatchMaxima( size.cy ), 1 );
	}

	// Second pass, produce the sprite locations in the append-consume buffer
	if( !m_produce.bind() )
		return S_FALSE;
	{
		BoundSrv<eStage::Compute> boundInput{ 3, m_tmpTexture.srv() };
		BoundUav appendConsume{ m_produce.data().bindOutput, m_tmpBuffer.uav() };
		context->Dispatch( dispatchProduce( size.cx ), dispatchProduce( size.cy ), 1 );
	}

	// Copy the counter from append-consume buffer to the state buffer
	context->CopyStructureCount( stateBuffer(), stateOffset() * 4, m_tmpBuffer.uav() );

	// Draw the sprites using DrawInstancedIndirect call
	if( !renderer.bindShaders( false ) )
		return S_FALSE;

	BoundPsResource boundSprite{ 3, stateData.m_sprite.srv() };
	BoundSrv<eStage::Vertex> boundSourceBuffer{ renderer.vertex().bindInput, m_tmpBuffer.srv() };

	rt.advance();
	CHECK( rt.writeToLast( true ) );

	omBlend( eBlend::Premultiplied );
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->DrawInstancedIndirect( stateBuffer(), stateOffset() * 4 );
	return S_OK;
}