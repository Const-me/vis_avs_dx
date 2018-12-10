#include "stdafx.h"
#include "RootEffect.h"
#include "../Resources/staticResources.h"

HRESULT RootEffect::renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat )
{
	CHECK( StaticResources::sourceData.update( visdata, isBeat ) );

	return E_NOTIMPL;
}