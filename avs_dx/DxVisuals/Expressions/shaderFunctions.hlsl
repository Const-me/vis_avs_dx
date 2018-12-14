#include "../Effects/VisCommon.fx"

// This file contains global functions available to the expression engine.
// This file gets parsed in design-time into individual functions: this approach saves compilation time, also allows to detect undefined functions earlier, without even calling HLSL compiler.

inline uint gettime()
{
    return getTickCount;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}