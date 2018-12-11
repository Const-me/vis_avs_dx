#include "stdafx.h"
#include "RootEffect.h"
#include "../Resources/staticResources.h"
#include "../../InteropLib/interop.h"
#include "../Render/Binder.h"

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

	const HRESULT hr = applyRecursively( [ & ]( EffectBase& e ) 
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

	return E_NOTIMPL;
}