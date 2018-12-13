#include "stdafx.h"
#include "Starfield.h"

IMPLEMENT_EFFECT( Starfield, C_StarField )

constexpr UINT nStars = 4096;
constexpr UINT csThreads = 256;
constexpr UINT nThreadGroups = nStars / csThreads;

HRESULT Starfield::initializedState()
{
	// Create the stars buffer
	if( !starsBuffer )
		CHECK( starsBuffer.create( 16, nStars ) );

	// Compile & run stars initial update shader
	Shader<eStage::Compute, StarInitCS> initShader{ StarInitCS::shaderTemplate() };
	Binder b;
	initShader.updateBindings( b );
	CHECK( initShader.compile( effectIncludes(), stateOffset() ) );

	initShader.bind();
	bindUav( initShader.data().bindStarsPosition, starsBuffer.uav() );
	context->Dispatch( nThreadGroups, 1, 1 );

	bindUav( initShader.data().bindStarsPosition );
	return S_OK;
}

HRESULT Starfield::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );

	// Calculate dots positions, with the CS
	const UINT uavSlot = data<eStage::Compute>().bindStarsPosition;
	bindUav( uavSlot, starsBuffer.uav() );
	context->Dispatch( nThreadGroups, 1, 1 );

	// Render the sprites
	const UINT srvSlot = data<eStage::Vertex>().bindStarsPosition;
	bindResource<eStage::Vertex>( srvSlot, starsBuffer.srv() );
	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->DrawIndexed( nStars, 0, 0 );

	return S_OK;
}