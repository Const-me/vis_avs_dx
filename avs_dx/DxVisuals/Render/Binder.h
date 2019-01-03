#pragma once
#include "Stage.h"

class Binder
{
	struct Slots
	{
		UINT srv, uav, cbuffer, sampler;
	};
	std::array<Slots, 4> m_data;

public:

	Binder();

	// Reserve the input slot, returns false if the result was unchanged.
	bool reserveInputSlot( UINT& result, eStage pipelineStage, char resourceType );
};