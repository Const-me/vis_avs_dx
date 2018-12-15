#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
// Such scale so 
static const float2 scaleToUniform = float2( 1, 1 );
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

sOutput main( sInput inputVertex )
{
    float x = inputVertex.pos.x;
    float y = inputVertex.pos.y;
    const float2 centered = scaleToUniform * inputVertex.pos;
    float d = length( centered );
    float r = atan2( centered.y, centered.x ) + ( M_PI / 2 );

	{
SHADER_CODE
    }

    sOutput res;
    res.pos = float4( x, y, 0, 1 );
    res.tc = inputVertex.tc;
    return res;
}