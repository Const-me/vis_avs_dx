#include "stdafx.h"
#include "Transition.h"
#include "Resources/staticResources.h"
#include <Utils/resizeHandler.h>
#include <Interop/interop.h>
#include <Interop/Utils/shutdown.h>
#include "effects.h"
#include "RootEffect.h"

// The critical section that guards renderers, linked from deep inside AVS.
extern CRITICAL_SECTION g_render_cs;

namespace
{
	CSize g_renderSize = CSize{ 0, 0 };
	CStringA g_renderSizeString = "float2(0,0)";
}

CSize getRenderSize()
{
	return g_renderSize;
}

const CStringA& getRenderSizeString()
{
	return g_renderSizeString;
}

void createTransition( unique_ptr<iTransition>& up )
{
	up = make_unique<Transition>();
}

Transition::Transition() :
	m_prepared( "prepare" ),
	m_rendered( "render" )
{ }

Transition::~Transition()
{
	logShutdown( "~Transition" );
}

HRESULT Transition::prepare( char visdata[ 2 ][ 2 ][ 576 ], int isBeat )
{
	// Upload visualization data to GPU
	CHECK( StaticResources::sourceData.update( visdata, isBeat ) );

	// Handle resize
	const CSize currentSize = getCurrentRenderSize();
	if( g_renderSize != currentSize )
	{
		g_renderSize = currentSize;
		g_renderSizeString.Format( "float2( %i, %i )", currentSize.cx, currentSize.cy );
		callResizeHandlers();

		m_viewport = CD3D11_VIEWPORT{ 0.0f, 0.0f, (float)currentSize.cx, (float)currentSize.cy };
	}
	context->RSSetViewports( 1, &m_viewport );
	m_prepared.mark();
	return S_OK;
}

inline RootEffect* getRoot( const C_RBASE *pRBase )
{
	return dynamic_cast<RootEffect*> ( getDxEffect( pRBase ) );
}

HRESULT Transition::renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *pRBase )
{
	if( checkShutdown() )
	{
		Sleep( 10 );
		return S_FALSE;
	}

	RootEffect* p = getRoot( pRBase );
	if( nullptr == p )
		return E_POINTER;

#if GPU_PROFILE
	gpuProfiler().frameStart();
#endif

	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );

		CHECK( p->renderRoot( 0 != isBeat, m_targets1, false ) );
	}

	m_rendered.mark();

	UnlockExternCs _unlock{ g_render_cs };
	CHECK( presentSingle( m_targets1.lastWritten() ) );

#if GPU_PROFILE
	gpuProfiler().frameEnd();
#endif

	return S_OK;
}

HRESULT Transition::renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *e1, const C_RBASE *e2, int trans, float sintrans )
{
	if( checkShutdown() )
	{
		Sleep( 10 );
		return S_FALSE;
	}

#if GPU_PROFILE
	gpuProfiler().frameStart();
#endif

	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );

		const bool beat = 0 != isBeat;

		RootEffect* p1 = getRoot( e1 ), *p2 = getRoot( e2 );
		if( nullptr == p1 || nullptr == p2 )
			return E_POINTER;

		CHECK( p1->renderRoot( beat, m_targets1, true ) );
		CHECK( p2->renderRoot( beat, m_targets2, true ) );
	}

	m_rendered.mark();

	UnlockExternCs _unlock{ g_render_cs };
	CHECK( presentTransition( m_targets1.lastWritten(), m_targets2.lastWritten(), trans, sintrans ) );

#if GPU_PROFILE
	gpuProfiler().frameEnd();
#endif
	
	return S_OK;
}

namespace
{
	unique_ptr<Transition> g_transition;
}
Transition* getTransitionInstance()
{
	return g_transition.get();
}

iTransition* getTransition()
{
	if( checkShutdown() )
	{
		Sleep( 10 );
		return nullptr;
	}
	return g_transition.get();
}

bool createTransitionInstance()
{
	if( g_transition )
		return false;
	g_transition = make_unique<Transition>();
	return true;
}

bool destroyTransitionInstance()
{
	if( !g_transition )
		return false;
	g_transition.reset();
	return true;
}