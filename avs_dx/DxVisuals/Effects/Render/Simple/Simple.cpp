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
}

float SimpleBase::AvsState::sampleV() const
{
	return sourceSampleV( source( effect ), channel( effect ) );
}

eSimpleStyle SimpleBase::AvsState::style() const
{
	if( effect & ( 1 << 6 ) )
		return eSimpleStyle::Dots;
	switch( effect & 3 )
	{
	case 0:
	case 3:
		return eSimpleStyle::Solid;
	}
	return eSimpleStyle::Lines;
}

static const array<float2, 3> s_positions =
{
	float2{ 0, 1 },				// top
	float2{ 0, -1 },			// bottom
	float2{ -0.5f, +0.5f, },	// center
};

float2 SimpleBase::AvsState::positionY() const
{
	const int ind = ( effect >> 4 ) & 3;
	assert( ind < 3 );
	return s_positions[ ind ];
}

HRESULT SimpleBase::StateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

// ==== Dots rendering ====

HRESULT DotsRendering::VsData::updateAvs( const AvsState& avs )
{
	BoolHr res;
	res.updateValue( y1y2, avs.positionY() );
	res.updateValue( sampleV, avs.sampleV() );
	return res;
}

HRESULT SimpleDotsFx::render( bool isBeat, RenderTargets& rt )
{
	const UINT pointsCount = vertex().pointsCount;

	CHECK( rt.writeToLast( false ) );
	if( !bindShaders( isBeat ) )
		return S_FALSE;

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->Draw( pointsCount, 0 );

	return S_OK;
}

// ==== Solid rendering ====

HRESULT SolidRendering::VsData::updateAvs( const AvsState& avs )
{
	return updateValue( y1y2, avs.positionY() );
}

HRESULT SolidRendering::PsData::updateAvs( const AvsState& avs )
{
	return updateValue( sampleV, avs.sampleV() );
}

HRESULT SimpleSolidFx::render( bool isBeat, RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	if( !bindShaders( isBeat ) )
		return S_FALSE;

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	context->Draw( 4, 0 );
	return S_OK;
}

// ==== Lines rendering ====

HRESULT LinesRendering::VsData::updateAvs( const AvsState& avs )
{
	BoolHr res;
	res.updateValue( y1y2, avs.positionY() );
	res.updateValue( sampleV, avs.sampleV() );
	return res;
}

HRESULT SimpleLinesFx::render( bool isBeat, RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	if( !bindShaders( isBeat ) )
		return S_FALSE;

	iaClearBuffer();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
	context->Draw( vertex().pointsCount + 2, 0 );
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
	const eSimpleStyle rs = avs->style();
	switch( rs )
	{
#define REPLACE_STYLE( eStyle, tEffect ) case eStyle: if( std::holds_alternative<tEffect>( m_impl ) ) return false; m_pImpl = &m_impl.emplace<tEffect>(); return true

		REPLACE_STYLE( eSimpleStyle::Dots, SimpleDotsFx );
		REPLACE_STYLE( eSimpleStyle::Solid, SimpleSolidFx );
		REPLACE_STYLE( eSimpleStyle::Lines, SimpleLinesFx );

#undef REPLACE_STYLE
	}
	__debugbreak();
	return false;
}

HRESULT Simple::shouldRebuildState()
{
	replaceStyleIfNeeded();

	// All rendering use the same state data i.e. cycling colors, no need to rebuild it when user switches the rendering style
	return EffectBase1::shouldRebuildState();
}

HRESULT Simple::updateParameters( Binder& binder )
{
	BoolHr hr = m_pImpl->updateParameters( binder, *avs, stateData );
	if( hr.succeeded() && hr.value() )
		hr.combine( m_pImpl->compileShaders( stateOffset() ) );
	return hr;
}

HRESULT Simple::render( bool isBeat, RenderTargets& rt )
{
	omBlend( eBlend::Premultiplied );
	return m_pImpl->render( isBeat, rt );
}