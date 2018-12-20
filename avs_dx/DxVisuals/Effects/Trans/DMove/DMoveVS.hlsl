#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
// Such scale so 
static const float2 scaleToUniform = float2( 0.8f, 0.6f );
static const bool rectangularCoords = false;
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
    float x = inputVertex.pos.x;
    float y = inputVertex.pos.y;
    const float2 centered = scaleToUniform * inputVertex.pos;
    float d = length( centered );
    float r = atan2( centered.y, centered.x ) + ( M_PI / 2 );

    {
SHADER_CODE
    }

    sOutput res;
    if( rectangularCoords )
        res.pos = float4( x, y, 0.5, 1 );
    else
    {
        r -= ( M_PI / 2 );
        float2 resultVec;
        sincos( r, resultVec.y, resultVec.x );
        resultVec *= d;
        const float2 antiScale = ( float2( 1, 1 ) / scaleToUniform );
        resultVec *= antiScale;
        res.pos = float4( resultVec, 0.5, 1 );
    }
    res.tc = inputVertex.tc;
    return res;
}