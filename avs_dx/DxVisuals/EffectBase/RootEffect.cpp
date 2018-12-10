#include "stdafx.h"
#include "RootEffect.h"
#include "../Resources/staticResources.h"
#include "../../InteropLib/interop.h"

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

	return E_NOTIMPL;
}