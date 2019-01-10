#include "stdafx.h"
#include "RenderTargets.h"
#include "staticResources.h"
#include <Interop/interop.h>
#include <Render/Binder.h>

inline void unbindTarget()
{
	context->OMSetRenderTargets( 0, nullptr, nullptr );
}

HRESULT RenderTargets::writeToLast( bool clear )
{
	RenderTarget& t = m_targets[ m_lastTarget ];
	if( !t )
	{
		// TODO: delay the creation. If the next effect will call writeToNext or blendToNext, the 1x1 black texture will do the same, only for free.
		CHECK( t.create() );
	}
	if( clear )
		t.clear();
	t.bindTarget();
	return S_OK;
}

HRESULT RenderTargets::writeToLastWithLogicOp()
{
	RenderTarget& t = m_targets[ m_lastTarget ];
	if( !t )
		CHECK( t.create() );
	t.bindTargetForLogicOp();
	return S_OK;
}

HRESULT RenderTargets::writeToNext( BoundPsResource& bound, bool clearNext )
{
	unbindTarget();

	RenderTarget& tRead = m_targets[ m_lastTarget ];
	if( tRead )
		bound.swap( tRead.psView() );
	else
		bindResource<eStage::Pixel>( Binder::psPrevFrame, StaticResources::blackTexture );

	m_lastTarget ^= 1;
	RenderTarget& tWrite = m_targets[ m_lastTarget ];
	if( !tWrite )
		CHECK( tWrite.create() );
	if( clearNext )
		tWrite.clear();
	tWrite.bindTarget();
	return S_OK;
}

HRESULT RenderTargets::blendToNext( BoundPsResource& bound )
{
	omBlend( eBlend::Premultiplied );
	RenderTarget& tRead = m_targets[ m_lastTarget ];
	if( !tRead )
		return writeToNext( bound, true );

	unbindTarget();
	m_lastTarget ^= 1;
	RenderTarget& tWrite = m_targets[ m_lastTarget ];
	if( !tWrite )
		CHECK( tWrite.create() );

	tRead.copyTo( tWrite );

	tWrite.bindTarget();
	bound.swap( tRead.psView() );
	return S_OK;
}

HRESULT RenderTargets::computeToNext( UINT readSlot, BoundSrv<eStage::Compute>& boundRead, UINT writeSlot, BoundUav& boundWrite )
{
	unbindTarget();
	RenderTarget& tRead = m_targets[ m_lastTarget ];

	if( tRead )
	{
		m_lastTarget ^= 1;
		RenderTarget& tWrite = m_targets[ m_lastTarget ];

		if( !tWrite )
			CHECK( tWrite.create() );
		CHECK( tWrite.createUav() );

		boundRead.swap( BoundSrv<eStage::Compute>{ readSlot, tRead.srv() } );
		boundWrite.swap( BoundUav{ writeSlot, tWrite.uav() } );
	}
	else
	{
		CHECK( tRead.create() );
		CHECK( tRead.createUav() );

		boundRead.swap( BoundSrv<eStage::Compute>{ readSlot, StaticResources::blackTexture } );
		boundWrite.swap( BoundUav{ writeSlot, tRead.uav() } );
	}
	return S_OK;
}