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
	context->CSSetShader( isBeat ? m_stateShaders.updateOnBeat : m_stateShaders.update, nullptr, 0 );
	bindUav( 0, m_state.uav() );
	context->Dispatch( 1, 1, 1 );

	CHECK( m_targets.writeToLast( clearfb() ) );

	CHECK( render( m_targets ) );

	return S_OK;
}

HRESULT RootEffect::buildState()
{
	// Collect effects state shaders
	std::vector<EffectStateShader> shaders;
	HRESULT hr = applyRecursively( [ & ]( EffectBase& e )
	{
		shaders.emplace_back( EffectStateShader{} );
		return e.buildState( *shaders.rbegin() );
	} );
	CHECK( hr );

	// Compile together
	UINT totalStateSize;
	CHECK( m_stateShaders.compile( shaders, totalStateSize ) );

	// Create state buffer
	if( totalStateSize != m_state.getSize() )
		CHECK( m_state.create( totalStateSize ) );

	// Run init shader
	bindShader<eStage::Compute>( m_stateShaders.init );
	bindUav( 0, m_state.uav() );
	StaticResources::sourceData.bind<eStage::Compute>( 0, 0 );
	context->Dispatch( 1, 1, 1 );

	return S_OK;
}