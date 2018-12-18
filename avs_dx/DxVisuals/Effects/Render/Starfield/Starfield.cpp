#include "stdafx.h"
#include "Starfield.h"

IMPLEMENT_EFFECT( Starfield, C_StarField );

// Max.limit of the count, the size of the buffers
constexpr UINT nStarsCapacity = 4096;
// Compute shader threads per group
constexpr UINT csThreads = 256;

HRESULT Starfield::initializedState()
{
	// Create the stars buffer
	if( !starsBuffer )
		CHECK( starsBuffer.create( 16, nStarsCapacity ) );

	// Compile & run stars initial update shader
	Shader<eStage::Compute, StarInitCS> initShader;
	Binder b;
	CHECK( initShader.update( b, nullptr, nullptr ) );
	CHECK( initShader.compile( effectIncludes(), stateOffset() ) );

	initShader.bind();
	bindUav( initShader.data().bindStarsPosition, starsBuffer.uav() );
	constexpr UINT nThreadGroupsTotal = nStarsCapacity / csThreads;
	context->Dispatch( nThreadGroupsTotal, 1, 1 );

	bindUav( initShader.data().bindStarsPosition );

	// Refresh stars count on the next render()
	prevStars = 0;
	nStars = 0;
	nComputeGroups = 0;

	return S_OK;
}

StarfieldStructs::StateData::StateData( const AvsState& s )
{
	onbeat = s.onbeat ? true : false;
	spdBeat = s.spdBeat;
	WarpSpeed = s.WarpSpeed;
	durFrames = s.durFrames;
}

HRESULT StarfieldStructs::VsData::updateAvs( const AvsState& ass )
{
	if( ass.color == m_color )
		return S_FALSE;
	starsColor = float3FromColor( m_color = ass.color );
	return S_OK;
}

HRESULT Starfield::render( RenderTargets& rt )
{
	if( prevStars != avs->MaxStars_set )
	{
		prevStars = avs->MaxStars_set;
		const CSize sz = getRenderSize();
		nStars = MulDiv( avs->MaxStars_set, sz.cx * sz.cy, 512 * 384 );
		nStars = std::min( nStars, nStarsCapacity );
		nComputeGroups = ( nStars + csThreads - 1 ) / csThreads;
	}

	CHECK( rt.writeToLast( false ) );
	renderer.bindShaders();

	// Calculate dots positions with the CS
	const UINT uavSlot = renderer.compute().bindStarsPosition;
	bindUav( uavSlot, starsBuffer.uav() );
	context->Dispatch( nComputeGroups, 1, 1 );
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