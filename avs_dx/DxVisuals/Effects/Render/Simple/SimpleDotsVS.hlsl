#include "VisCommon.fx"

#ifndef AVS_SHADER
#define BIND_DOTS_POSITIONS t4
// <macro-resource name="bindDots" macro="BIND_DOTS_POSITIONS" />
#endif

StructuredBuffer<float2> positiions : register( BIND_DOTS_POSITIONS );

struct SimpleVertex
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

SimpleVertex main( uint id : SV_VertexID )
{
    SimpleVertex r;
    r.pos = float4( positiions[ id ], 0, 1 );
    r.color = float4( stateFloat3( 1 ), 1 );
    return r;
}