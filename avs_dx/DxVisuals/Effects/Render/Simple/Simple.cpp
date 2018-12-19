#include "stdafx.h"
#include "Simple.h"

// Class factory infrastructure

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

// ==== Common stuff for all styles ====

namespace
{
	eSource source( int effect )
	{
		return ( effect & 2 ) ? eSource::Wave : eSource::Spectrum;
	}

	eChannel channel( int effect )
	{
		const int which_ch = ( effect >> 2 ) & 3;
		if( 0 == which_ch ) return eChannel::Left;
		if( 1 == which_ch ) return eChannel::Right;
		return eChannel::Center;
	}

	eSimpleRenderStyle getRenderStyle( int effect )
	{
		if( effect & ( 1 << 6 ) )
			return eSimpleRenderStyle::Dots;
		switch( effect & 3 )
		{
		case 0:
		case 3:
			return eSimpleRenderStyle::Solid;
		}
		return eSimpleRenderStyle::Lines;
	}
}

float SimpleBase::AvsState::sampleV() const
{
	return sourceSampleV( source( effect ), channel( effect ) );
}

HRESULT SimpleBase::StateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

// ==== Dots rendering ====

HRESULT SimpleDotsFx::render( RenderTargets& rt )
{
	const UINT pointsCount = renderer.vertex().pointsCount;

	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->Draw( pointsCount, 0 );

	return S_OK;
}

// ==== Solid rendering ====

HRESULT SolidRendering::VsData::updateAvs( const AvsState& avs )
{
	// TODO: positions
	return S_FALSE;
}

HRESULT SolidRendering::PsData::updateAvs( const AvsState& avs )
{
	return updateValue( sampleV, avs.sampleV() );
}

HRESULT SimpleSolidFx::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );
	return S_OK;
}

// ==== Lines rendering ====

HRESULT SimpleLinesFx::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
	context->Draw( renderer.vertex().pointsCount + 2, 0 );
	return S_OK;
}

// ==== The effect itself ====

Simple::Simple( AvsState *pState ) :
	EffectBase1( pState ),
	m_impl( std::monostate{} )
{
	replaceStyleIfNeeded();
}

bool Simple::replaceStyleIfNeeded()
{
	const eSimpleRenderStyle rs = getRenderStyle( avs->effect );
	switch( rs )
	{
#define REPLACE_STYLE( eStyle, tEffect ) case eStyle: if( std::holds_alternative<tEffect>( m_impl ) ) return false; m_pImpl = &m_impl.emplace<tEffect>( avs ); return true

		REPLACE_STYLE( eSimpleRenderStyle::Dots, SimpleDotsFx );
		REPLACE_STYLE( eSimpleRenderStyle::Solid, SimpleSolidFx );
		REPLACE_STYLE( eSimpleRenderStyle::Lines, SimpleLinesFx );

#undef REPLACE_STYLE
	}
	__debugbreak();
	return false;
}

HRESULT Simple::shouldRebuildState()
{
	if( replaceStyleIfNeeded() )
		m_pImpl->setStateOffset( stateOffset() );

	// All rendering use the same state data i.e. cycling colors, no need to rebuild it when user switches the rendering style
	return EffectBase1::shouldRebuildState();
}

void Simple::setStateOffset( UINT off )
{
	EffectBase1::setStateOffset( off );
	m_pImpl->setStateOffset( off );
}