#include "stdafx.h"
#include "Simple.h"

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

HRESULT SimpleBase::StateData::update( const AvsState& ass )
{
	if( std::equal( colors.begin(), colors.end(), ass.colors, ass.colors + ass.num_colors ) )
		return S_FALSE;
	colors.assign( ass.colors, ass.colors + ass.num_colors );
	return S_OK;
}

HRESULT Simple::render( RenderTargets& rt )
{
	// TODO: change based on the size
	constexpr UINT dotsCount = 768;
	if( !dotsBuffer )
		CHECK( dotsBuffer.create( sizeof( Vector2 ), dotsCount ) );

	bindShaders();


	return E_NOTIMPL;
}