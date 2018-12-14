#include "stdafx.h"
#include "DynamicMovement.h"

IMPLEMENT_EFFECT( DynamicMovement, C_DMoveClass )


HRESULT DynamicMovement::render( RenderTargets& rt )
{
	const UINT psReadSlot = data<eStage::Pixel>().bindPrevFrame;
	CHECK( rt.writeToNext( psReadSlot, true ) );

	return E_NOTIMPL; 
}