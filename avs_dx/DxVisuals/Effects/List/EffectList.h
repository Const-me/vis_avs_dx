#pragma once
#include "../../EffectBase/EffectListBase.h"

class EffectList : public EffectListBase
{
public:
	EffectList( AvsState* pState ) : EffectListBase( pState ) { }

	const Metadata& metadata() override;
};