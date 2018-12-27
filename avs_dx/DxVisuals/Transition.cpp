#include "stdafx.h"
#include "Transition.h"
#include "Resources/staticResources.h"
#include <Utils/resizeHandler.h>
#include "../InteropLib/interop.h"
#include "../InteropLib/Utils/shutdown.h"

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

void createTransition( std::unique_ptr<iTransition>& up )
{
	up = std::make_unique<Transition>();
}

Transition::~Transition()
{ }

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
	}
	return S_OK;
}

HRESULT Transition::renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e )
{
	if( checkShutdown() )
		return S_FALSE;

	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );

		CHECK( e.renderRoot( 0 != isBeat, m_targets1 ) );
	}

	UnlockExternCs _unlock{ g_render_cs };
	CHECK( presentSingle( m_targets1.lastWritten() ) );

	return S_OK;
}

HRESULT Transition::renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e1, iRootEffect &e2, int trans, float sintrans ) 
{
	if( checkShutdown() )
		return S_FALSE;

	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );

		const bool beat = 0 != isBeat;

		CHECK( e1.renderRoot( beat, m_targets1 ) );
		CHECK( e2.renderRoot( beat, m_targets2 ) );
	}

	UnlockExternCs _unlock{ g_render_cs };
	CHECK( presentTransition( m_targets1.lastWritten(), m_targets2.lastWritten(), trans, sintrans ) );
	
	return S_OK;
}