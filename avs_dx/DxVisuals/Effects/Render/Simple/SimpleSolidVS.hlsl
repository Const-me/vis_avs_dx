#include "VisCommon.fx"

#ifndef AVS_SHADER
// Centered position
static const float SIMPLE_Y1 = 0.25;
static const float SIMPLE_Y2 = 0.75;
#endif

struct SimpleVertex
{
    float4 pos : SV_Position;
    float2 tc : TEXCOORD0;
    float4 color : color0;
};

SimpleVertex main( uint id : SV_VertexID )
{
    const float2 xy = float2(id & 1, id >> 1);
    SimpleVertex res;
    res.pos.xy = lerp(float2(-1, SIMPLE_Y1), float2(+1, SIMPLE_Y2), xy);
    res.pos.zw = float2(0, 1);
    res.tc.x = xy.x;
    res.tc.y = lerp(-1, +1, xy.y);
    res.color = float4( stateFloat3( 1 ), 1 );
    return res;
}