#include "stdafx.h"
#include "RootEffect.h"
#include "../Resources/staticResources.h"
#include "../../InteropLib/interop.h"
#include "../Render/Binder.h"
#include "../Render/StateShaders.h"

// The critical section that guards renderers, linked from deep inside AVS.
extern CRITICAL_SECTION g_render_cs;

HRESULT RootEffect::renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat )
{
	{
		CSLock __lock( renderLock );

		// Upload visualization data to GPU
		CHECK( StaticResources::sourceData.update( visdata, isBeat ) );

		CHECK( renderEffects( 0 != isBeat ) );
	}

	CHECK( present( m_targets.lastWritten() ) );

	return S_OK;
}

HRESULT RootEffect::renderEffects( bool isBeat )
{
	// Handle resize
	{
		const CSize currentSize = getRenderSize();
		if( m_renderSize != currentSize )
		{
			m_targets.destroy();
			m_renderSize = currentSize;
		}
	}

	// Collect the effects in the local list
	{
		LockExternCs __lock{ g_render_cs };
		const bool listChanged = updateList();
		const bool stateChanged = shouldRebuildState();
		if( listChanged || stateChanged )
		{
			CHECK( buildState() );
		}

		Binder binder;
		CHECK( updateParameters( binder ) );
	}

	// Run a state update shader
	context->CSSetShader( isBeat ? m_updateStateBeat : m_updateState, nullptr, 0 );
	csSetUav( m_state.uav(), 0 );
	context->Dispatch( 1, 1, 1 );

	CHECK( m_targets.writeToLast( clearfb() ) );

	CHECK( render( m_targets ) );

	return S_OK;
}

HRESULT RootEffect::buildState()
{
	CAtlMap<CStringA, int> globals;
	std::vector<CStringA> mainPieces;
	bool useBeat = false;
	int stateBufferOffset = 0;

	HRESULT hr = applyRecursively( [ & ]( EffectBase& e )
	{
		CStringA hlsl;
		bool beat = false;
		int thisSize = 0;
		CHECK( e.buildState( stateBufferOffset, thisSize, hlsl, beat, globals ) );

		stateBufferOffset += thisSize * 4;

		if( hlsl.GetLength() > 0 )
			mainPieces.push_back( hlsl );

		useBeat = useBeat || beat;
		return S_OK;
	} );
	CHECK( hr );

	CStringA hlsl = R"fffuuu(
#include "FrameGlobalData.fx"

RWByteAddressBuffer effectStates : register(u0);
)fffuuu";

	for( POSITION pos = globals.GetStartPosition(); nullptr != pos; )
	{
		CStringA s = globals.GetNextKey( pos );
		hlsl += s;
		hlsl += "\r\n";
	}

	hlsl += R"fffuuu(
[numthreads( 1, 1, 1 )]
void main()
{
)fffuuu";

	for( auto& s : mainPieces )
	{
		hlsl += s;
		hlsl += "\r\n";
	}
	hlsl += "}";

	std::vector<uint8_t> dxbc;
	CStringA errors;
	hr = Hlsl::compile( eStage::Compute, hlsl, dxbc, errors );
	if( FAILED( hr ) )
	{
		logError( hr, "Error compiling state shader." );
		return hr;
	}

	CHECK( createShader( dxbc, m_updateState ) );

	// TODO: if useBeat == true, change defines and compile again.
	m_updateStateBeat = m_updateState;


	const UINT newStateSize = (UINT)stateBufferOffset / 4;
	if( newStateSize != m_state.getSize() )
		CHECK( m_state.create( newStateSize ) );

	return S_OK;
}