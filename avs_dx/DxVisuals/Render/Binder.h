#pragma once
#include "EffectsResources.h"
#include "Stage.h"

class Binder
{
	struct Slots
	{
		UINT srv, uav, cbuffer;
	};
	std::array<Slots, 4> m_data;

public:

	Binder();

	UINT reserveInputSlot( eStage pipelineStage, char resourceType );
};