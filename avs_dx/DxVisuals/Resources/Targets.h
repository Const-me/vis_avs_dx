#pragma once
#include "RenderTarget.h"

class Targets
{
public:
};

class TargetHandle
{
	const CSize size;
public:
	bool expired() const;
};

extern Targets targets;