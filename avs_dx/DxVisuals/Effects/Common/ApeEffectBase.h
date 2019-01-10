#pragma once
#include "EffectBase.h"

class C_RBASE;

// An effect that's implemented outside of vis_avs.dll, in an external APE.
class ApeEffectBase: public EffectBase
{
public:
	using pfnCreateEffect = HRESULT( *)( C_RBASE* pEffect );

	struct Metadata: public EffectBase::Metadata
	{
		Metadata() = delete;
		Metadata( const char* name, const char* apeName, pfnCreateEffect classFactory );
	};

	using AvsState = C_RBASE;

	static HRESULT create( HINSTANCE hDllInstance, const char* nameEffect, C_RBASE* pThis );
};