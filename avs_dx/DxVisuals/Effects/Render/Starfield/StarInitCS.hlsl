#include "VisCommon.fx"
#include "Star.fx"
#ifndef AVS_SHADER
#define BIND_STARS_POSITIONS u4
// <macro-resource name="bindStarsPosition" macro="BIND_STARS_POSITIONS" type="uav" />
#endif
RWStructuredBuffer<StarFormat> stars : register(BIND_STARS_POSITIONS);

[numthreads( 256, 1, 1 )]
void main( in uint3 threadId : SV_DispatchThreadID )
{
    const uint id = threadId.x;
    uint rng_state = srand( ~id );

    StarFormat s;
    s.position.xy = randomPos( rng_state );
    s.position.z = randf( rng_state );
    s.speed = ( randf( rng_state ) * 9 + 1 ) * 0.1;
    stars[ id ] = s;
}