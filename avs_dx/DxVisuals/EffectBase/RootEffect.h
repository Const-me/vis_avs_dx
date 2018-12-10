#pragma once
#include "EffectListBase.h"

class RootEffect : public EffectListBase
{
public:
	RootEffect( AvsState* pState ) : EffectListBase( pState ) { }

	HRESULT renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat ) override;
};