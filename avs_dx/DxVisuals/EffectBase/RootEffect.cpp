#include "stdafx.h"
#include "RootEffect.h"
#include "../Resources/staticResources.h"
#include "../../InteropLib/interop.h"

// The critical section that guards renderers, linked from deep inside AVS.
extern CRITICAL_SECTION g_render_cs;

HRESULT RootEffect::renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat )
{
	{
		CSLock __lock( renderLock );

		// Upload visualization data to GPU
		CHECK( StaticResources::sourceData.update( visdata, isBeat ) );

		CHECK( renderEffects() );
	}

	CHECK( present( m_targets[ m_lastTarget ] ) );

	return S_OK;
}

HRESULT RootEffect::renderEffects()
{
	// Handle resize
	{
		const CSize currentSize = getRenderSize();
		if( m_renderSize != currentSize )
		{
			for( auto& t : m_targets )
				t.destroy();
			m_renderSize = currentSize;
		}
	}

	auto& target = m_targets[ m_lastTarget ];
	if( !target )
		CHECK( target.create( m_renderSize ) );
	if( clearfb() )
		target.clear();



	return E_NOTIMPL;
}