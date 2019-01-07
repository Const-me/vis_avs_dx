#pragma once
// Interface for DxVisuals code to access the effects
#include "EffectBase/EffectBase.h"

class C_RBASE;

void addNewEffect( const C_RBASE* pRBase, unique_ptr<EffectBase>&& fx );

EffectBase* getDxEffect( const C_RBASE* pRBase );

void destroyAllEffects();