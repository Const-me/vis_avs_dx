#pragma once
#include "Stage.h"

class Binder
{
	struct Slots
	{
		UINT srv, uav, cbuffer, sampler;
	};
	using BindingsData = std::array<Slots, 4>;

	static const BindingsData m_staticBinds;
	static BindingsData staticallyBoundSlots();

	BindingsData m_data;

public:

	static constexpr UINT psPrevFrame = 3;

	Binder();

	// Reserve the input slot, returns false if the result was unchanged.
	bool reserveInputSlot( UINT& result, eStage pipelineStage, char resourceType );
};