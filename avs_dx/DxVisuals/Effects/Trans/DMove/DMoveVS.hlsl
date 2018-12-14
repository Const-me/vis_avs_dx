#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
#endif
SHADER_GLOBALS

struct sInput
{
    float2 pos: SV_Position;
    float2 tc : TEXCOORD0;
};

struct sOutput
{
    float4 pos : SV_Position;
    float2 tc : TEXCOORD0;
};

sOutput main( sInput v )
{
    sOutput res;
    res.pos = float4( v.pos, 0, 1 );
    res.tc = v.tc;
    return res;
}