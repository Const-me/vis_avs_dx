#include "stdafx.h"
#include "RootEffect.h"
#include "Resources/staticResources.h"
#include <Interop/interop.h>
#include "Render/Binder.h"
#include "Render/StateShaders.h"
#include "Effects/List/SavedContext.hpp"

HRESULT RootEffect::renderRoot( bool isBeat, RenderTargets& rt, bool rebindResources )
{
	ProfilerLevel plvl;
	// Collect the effects in the local list
	const bool listChanged = updateList( this );
	bool paramsChanged = listChanged;
	const BoolHr stateChanged = shouldRebuildState();
	if( stateChanged.failed() )
		return stateChanged;
	if( listChanged || stateChanged.value() || !m_stateShaders )
	{
		CHECK( buildState() );
		paramsChanged = true;
	}

	SavedContext __context{ m_savedGlobalBuffers };
	g_line_blend_mode = 0;
	
	{
		Binder binder;
		const HRESULT hr = updateParameters( binder );
		SILENT_CHECK( hr );
		if( hr == S_OK )
			paramsChanged = true;
	}

	// Run a state update shader
	SILENT_CHECK( m_stateShaders.bindUpdateShader( isBeat ) );

	bindGlobalResource( 2 );
	bindUav( 0, m_state.uav() );

	context->Dispatch( 1, 1, 1 );

	bindUav( 0 );
	bindGlobalResource( 2, m_state.srv() );

	const bool clear = clearfb();

	if( rt.lastWritten() )
	{
		if( clearfb() )
			rt.lastWritten().clear();
		else
			CHECK( fadeRenderTarget( rt ) );
	}

	if( paramsChanged || rebindResources )
		bindResources();

	CHECK( render( isBeat, rt ) );
	CHECK( completedRendering() );

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
	vector<EffectStateShader> shaders;
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

	if( shaders.empty() )
		return S_FALSE;

	// Compile together
	UINT totalStateSize;
	CHECK( m_stateShaders.compile( shaders, totalStateSize ) );

	if( 0 == totalStateSize )
		return S_FALSE;

	// Create state buffer
	if( totalStateSize != m_state.getSize() )
		CHECK( m_state.create( totalStateSize ) );

	// Run state init shader
	SILENT_CHECK( m_stateShaders.bindInitShader() );
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