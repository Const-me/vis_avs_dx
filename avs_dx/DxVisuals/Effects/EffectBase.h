#pragma once
#include "../Render/EffectBuilder.h"

/* enum eEffectFlags : uint32_t
{
	// The effect has some code to update it's state each frame. This implies non-zero state size.
	hasUpdate = 1,
	// The effect has some extra code to update it's state when the frame has a beat detected.
	hasBeat = 2,

	// The effect has some extra compute code to run after state updates
	hasExtraCompute = 0x10,
}; */

class EffectBase
{
public:
	/* struct EffectMetadata
	{
		const uint32_t stateSize;
		const bool hasUpdate;
		const bool hasBeat;
		const bool hasExtraCompute;
	}; */

	// Size of per-effect instance data, the count of dwords/floats
	// virtual uint32_t stateSize() { return 0; }

	// virtual HRESULT frameUpdateState( int stateOffset, Hlsl::FunctionBuilder& hlsl ) { return E_NOTIMPL; }
	// virtual HRESULT frameUpdateStateOnBeat( int stateOffset, Hlsl::FunctionBuilder& hlsl ) { return E_NOTIMPL; }

	virtual HRESULT build( EffectBuilder &builder ) = 0;
};

template<class T>
class EffectBase2 : public EffectBase
{

};