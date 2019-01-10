#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
#define BIND_TABLE u3
// <macro-resource name="bindTable" macro="BIND_TABLE" type="uav" />
#endif
RWTexture1D<float4> table : register(BIND_TABLE);

SHADER_GLOBALS

[numthreads(256, 1, 1)]
void main( uint3 globalThread : SV_DispatchThreadID )
{
    float red = (float) globalThread.x * ( 1.0 / 1023.0 );
    float green = red;
    float blue = red;

#if NEEDS_RNG
    uint rng_state = avs_rand_init( getRandomSeed( globalThread.x ) );
#endif

SHADER_CODE

    float3 rgb = float3( red, green, blue );
    table[ globalThread.x ] = float4( saturate( rgb ), 1 );
}