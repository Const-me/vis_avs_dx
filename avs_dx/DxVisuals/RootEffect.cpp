#include "stdafx.h"
#include "RootEffect.h"
#include "Resources/staticResources.h"
#include "../InteropLib/interop.h"
#include "Render/Binder.h"
#include "Render/StateShaders.h"

// The critical section that guards renderers, linked from deep inside AVS.
extern CRITICAL_SECTION g_render_cs;

HRESULT RootEffect::renderRoot( bool isBeat, RenderTargets& rt )
{
	// Collect the effects in the local list
	{
		LockExternCs __lock{ g_render_cs };
		const bool listChanged = updateList();
		const BoolHr stateChanged = shouldRebuildState();
		if( stateChanged.failed() )
			return stateChanged;
		if( listChanged || stateChanged.value() || !m_stateShaders )
		{
			CHECK( buildState() );
		}

		Binder binder;
		CHECK( updateParameters( binder ) );
	}

	// Run a state update shader
	context->CSSetShader( isBeat ? m_stateShaders.updateOnBeat : m_stateShaders.update, nullptr, 0 );

	bindGlobalResource( 2 );
	bindUav( 0, m_state.uav() );

	context->Dispatch( 1, 1, 1 );

	bindUav( 0 );
	bindGlobalResource( 2, m_state.srv() );

	CHECK( rt.writeToLast( clearfb() ) );

	CHECK( render( rt ) );

	return S_OK;
}

HRESULT RootEffect::clearRenders()
{
	clearEffects();
	return S_OK;
}

HRESULT RootEffect::buildState()
{
	// Collect effects state shaders
	std::vector<EffectStateShader> shaders;
	UINT stateOffset = 0;
	HRESULT hr = applyRecursively( [ & ]( EffectBase& e )
	{
		shaders.emplace_back( EffectStateShader{} );
		EffectStateShader &ess = *shaders.rbegin();
		
		const HRESULT hr = e.buildState( ess );
		e.setStateOffset( stateOffset );
		stateOffset += ess.stateSize;
		return hr;
	} );
	CHECK( hr );

	// Compile together
	UINT totalStateSize;
	CHECK( m_stateShaders.compile( shaders, totalStateSize ) );

	// Create state buffer
	if( totalStateSize != m_state.getSize() )
		CHECK( m_state.create( totalStateSize ) );

	// Run state init shader
	bindShader<eStage::Compute>( m_stateShaders.init );
	bindGlobalResource( 2 );
	bindUav( 0, m_state.uav() );
	context->Dispatch( 1, 1, 1 );

	bindUav( 0 );
	bindGlobalResource( 2, m_state.srv() );

	// Invoke initializedState() method for all effects
	hr = applyRecursively( [ & ]( EffectBase& e ) { return e.initializedState(); } );
	CHECK( hr );

	return S_OK;
}