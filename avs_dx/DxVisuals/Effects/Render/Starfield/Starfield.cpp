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
	Shader<eStage::Compute, StarInitCS> initShader;
	Binder b;
	CHECK( initShader.update( b, nullptr, nullptr ) );
	CHECK( initShader.compile( effectIncludes(), stateOffset() ) );

	initShader.bind();
	bindUav( initShader.data().bindStarsPosition, starsBuffer.uav() );
	context->Dispatch( nThreadGroups, 1, 1 );

	bindUav( initShader.data().bindStarsPosition );
	return S_OK;
}

HRESULT StarfieldStructs::VsData::update( const AvsState& ass )
{
	if( ass.color == m_color )
		return S_FALSE;
	starsColor = float3FromColor( m_color = ass.color );
	return S_OK;
}

HRESULT Starfield::render( RenderTargets& rt )
{
	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	// Calculate dots positions with the CS
	const UINT uavSlot = renderer.compute().bindStarsPosition;
	bindUav( uavSlot, starsBuffer.uav() );
	context->Dispatch( nThreadGroups, 1, 1 );
	bindUav( uavSlot );

	// Render the sprites
	const UINT srvSlot = renderer.vertex().bindStarsPosition;
	bindResource<eStage::Vertex>( srvSlot, starsBuffer.srv() );
	iaClearBuffers();
	context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	context->Draw( nStars, 0 );
	bindResource<eStage::Vertex>( srvSlot );

	return S_OK;
}