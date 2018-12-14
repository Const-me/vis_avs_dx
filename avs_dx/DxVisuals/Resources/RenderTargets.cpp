#include "stdafx.h"
#include "RenderTargets.h"
#include "staticResources.h"

HRESULT RenderTargets::writeToLast( bool clear )
{
	RenderTarget& t = m_targets[ m_lastTarget ];
	if( !t )
		CHECK( t.create( getRenderSize() ) );
	if( clear )
		t.clear();
	t.bindTarget();
	return S_OK;

}

HRESULT RenderTargets::writeToNext( UINT readPsSlot, bool clearNext )
{
	RenderTarget& tRead = m_targets[ m_lastTarget ];
	if( tRead )
		tRead.bindView( readPsSlot );
	else
		bindResource<eStage::Pixel>( readPsSlot, StaticResources::blackTexture );

	m_lastTarget ^= 1;
	RenderTarget& tWrite = m_targets[ m_lastTarget ];
	if( !tWrite )
		CHECK( tWrite.create( getRenderSize() ) );
	if( clearNext )
		tWrite.clear();
	tWrite.bindTarget();
	return S_OK;
}

HRESULT RenderTargets::blendToNext( UINT readPsSlot )
{
	RenderTarget& tRead = m_targets[ m_lastTarget ];
	if( !tRead )
		return writeToNext( readPsSlot, true );
	tRead.bindView( readPsSlot );

	m_lastTarget ^= 1;
	RenderTarget& tWrite = m_targets[ m_lastTarget ];
	if( !tWrite )
		CHECK( tWrite.create( getRenderSize() ) );
	tRead.copyTo( tWrite );

	tRead.bindView( readPsSlot );
	tWrite.bindTarget();
	return S_OK;
}