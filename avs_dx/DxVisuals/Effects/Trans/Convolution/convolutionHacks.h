#pragma once
class C_RBASE;

struct KernelData
{
	bool wrap;
	array<float, 49> values;
};

KernelData getKernelData( const C_RBASE* pConvFx );

struct MiscData
{
	bool enabled, twoPasses;
};

MiscData getMiscData( const C_RBASE* pConvFx );