#include "VisCommon.fx"

#ifndef AVS_SHADER
static const float2 y1y2 = float2( -0.5f, 0.5f );
#endif

struct SimpleVertex
{
    float2 tc : TEXCOORD0;
    float4 color : COLOR0;
    float4 pos : SV_Position;
};

SimpleVertex main( uint id : SV_VertexID )
{
    const uint yXor = ( y1y2.y > y1y2.x ) ? 1 : 0;
    const float2 xy = float2( id & 1, ( id >> 1 ) ^ yXor );
    SimpleVertex res;
    res.pos.xy = lerp( float2( -1, y1y2.x ), float2( +1, y1y2.y ), xy );
    res.pos.zw = float2(0, 1);
    res.tc = xy;
    res.color = float4( stateFloat3( 1 ), 1 );
    return res;
}