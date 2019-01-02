#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
static const float2 scaleToUniform = float2( 0.8f, 0.6f );
#endif
#define M_PI 3.141592653589793238
SHADER_GLOBALS

struct sInput
{
    float2 pos : SV_Position;
    float2 tc : TEXCOORD0;
};

struct sOutput
{
    float2 tc : TEXCOORD0;
    float4 pos : SV_Position;
};

sOutput main( sInput inputVertex )
{
    float2 xy = inputVertex.tc * 2 - float2( 1, 1 );
    const float2 centered = scaleToUniform * xy;
    float d = length( centered );
    const float2 dir = centered / d;

#if NEEDS_RNG
    uint rng_state = avs_rand_init( getRandomSeed( inputVertex.tc ) );
#endif

    {
SHADER_CODE
    }

    sOutput res;
    const float2 resultVec = dir * d;
    const float2 antiScale = ( float2( 0.5, 0.5 ) / scaleToUniform );
    res.tc = resultVec * antiScale + float2( 0.5, 0.5 );
    res.pos = float4( inputVertex.pos, 0.5, 1 );
    return res;
}