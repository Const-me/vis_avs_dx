#include "VisCommon.hlsli"
#include "Fountain.hlsli"
#ifndef AVS_SHADER
#define BIND_PARTICLES u4
// <macro-resource name="bindParticles" macro="BIND_PARTICLES" type="uav" />
#endif
RWStructuredBuffer<FountainPoint> particles : register(BIND_PARTICLES);

[numthreads( NUM_ROT_DIV, 1, 1 )]
void main( uint3 localThread : SV_GroupThreadID, uint3 globalThread : SV_DispatchThreadID )
{
    const uint angle = localThread.x;

    FountainPoint p;
    sincos( radians( angle * ( 360.0 / NUM_ROT_DIV ) ), p.xyDir.y, p.xyDir.x );
    p.color = 0;
    p.position = 0;
    p.speed = 0;
    particles[ globalThread.x ] = p;
}