#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#define AVS_RENDER_SIZE float2( 1280, 720 )
static const float fuzzyRadius = 4;
#endif
Texture2D<float4> texPrevFrame : register( BIND_PREV_FRAME );

inline float avs_rand( inout uint rng_state, float maxVal )
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= ( rng_state << 13 );
    rng_state ^= ( rng_state >> 17 );
    rng_state ^= ( rng_state << 5 );

    const float res = float( rng_state ) * ( 1.0 / 4294967296.0 );
    return maxVal * res;
}

inline float2 randomInCircle( inout uint rng_state )
{
    float r = avs_rand( rng_state, fuzzyRadius );
    float a = avs_rand( rng_state, radians( 360 ) );
    float2 res;
    sincos( a, res.y, res.x );
    return res * r;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const uint2 pos = uint2( screenSpace.xy );
    uint rng_state = wang_hash( pos.x ) ^ wang_hash( pos.y ) ^ wang_hash( getTickCount );
    float2 loadPos = screenSpace.xy + randomInCircle( rng_state );
    loadPos = clamp( loadPos, float2( 0, 0 ), AVS_RENDER_SIZE );
    return texPrevFrame.Load( int3( int2( loadPos ), 0 ) );
}