#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_CODE
// <macro-code name="shaderCode" macro="SHADER_CODE" />
#endif

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