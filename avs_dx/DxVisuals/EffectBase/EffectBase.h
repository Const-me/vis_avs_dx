#pragma once
#include "../iEffect.h"
class EffectStateBuilder;

class EffectBase: public iEffect
{
public:
	struct Metadata
	{
		const char* const name;
		const bool isList;
		Metadata( const char* n, bool il ) : name( n ), isList( il ) { }
	};

	virtual const Metadata& metadata() = 0;

	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;
};