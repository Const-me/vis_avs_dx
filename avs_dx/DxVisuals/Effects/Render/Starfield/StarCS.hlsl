#include "VisCommon.hlsli"
#include "Star.hlsli"
#ifndef AVS_SHADER
#define BIND_STARS_POSITIONS u4
// <macro-resource name="bindParticles" macro="BIND_STARS_POSITIONS" type="uav" />
#endif
RWStructuredBuffer<StarFormat> stars : register(BIND_STARS_POSITIONS);

inline bool isStarOk(float3 pos)
{
    bool result;
	if( pos.z <= 0 )
        result = false;
	else
    {
        const float2 p2 = abs( pos.xy / pos.z );
        result = p2.x < 1 && p2.y < 1;
    }
    return result;
}

inline void createStar( uint id, inout StarFormat s )
{
    uint rng_state = srand( id );
    s.position.xy = randomPos( rng_state );
    s.position.z = 1;
}

[numthreads( 256, 1, 1 )]
void main( in uint3 threadId : SV_DispatchThreadID )
{
    const uint id = threadId.x;
    StarFormat s = stars[ id ];

    const float CurrentSpeed = stateFloat( 1 );
    s.position.z -= s.speed * CurrentSpeed * ( 1.0 / 255 );
    if( isStarOk( s.position ) )
    {
        stars[ id ] = s;
        return;
    }

    createStar( id, s );
    stars[ id ] = s;
}