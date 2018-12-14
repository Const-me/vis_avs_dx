#include "../Effects/VisCommon.fx"

inline uint gettime()
{
    return getTickCount;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}