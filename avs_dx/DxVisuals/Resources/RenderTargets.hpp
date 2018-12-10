#pragma once
#include "RenderTarget.h"
#include "../../InteropLib/interop.h"

class RenderTargets
{
	std::array<RenderTarget, 2> m_targets;
	size_t m_lastTarget = 0;

public:
	void destroy()
	{
		for( auto& t : m_targets )
			t.destroy();
	}
	HRESULT writeToLast( bool clear )
	{
		RenderTarget& t = m_targets[ m_lastTarget ];
		if( !t )
			CHECK( t.create( getRenderSize() ) );
		if( clear )
			t.clear();
		t.bindTarget();
		return S_OK;

	}
	HRESULT writeToNext( UINT readPsSlot, bool clear )
	{
		RenderTarget& tRead = m_targets[ m_lastTarget ];
		tRead.bindView( readPsSlot );

		m_lastTarget ^= 1;
		RenderTarget& tWrite = m_targets[ m_lastTarget ];
		if( !tWrite )
			CHECK( tWrite.create( getRenderSize() ) );
		if( clear )
			tWrite.clear();
		tWrite.bindTarget();
		return S_OK;
	}
	const RenderTarget& lastWritten() const
	{
		return m_targets[ m_lastTarget ];
	}
};