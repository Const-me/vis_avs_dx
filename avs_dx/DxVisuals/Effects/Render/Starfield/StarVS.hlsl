#include "VisCommon.fx"
#include "Star.fx"
#ifndef AVS_SHADER
#define BIND_STARS_POSITIONS t4
// <macro-resource name="bindStarsPosition" macro="BIND_STARS_POSITIONS" />
static const float3 starsColor = float3( 0.333f, 0.333f, 1.0f );
#endif

StructuredBuffer<StarFormat> stars : register(BIND_STARS_POSITIONS);

struct SimpleVertex
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

SimpleVertex main( uint id : SV_VertexID )
{
    StarFormat s = stars[ id ];
    SimpleVertex r;
    r.pos = float4( s.position.xy / s.position.z, 0.5, 1 );
    const float c = ( 1.0 - s.position.z ) * s.speed;
    r.color = float4( starsColor * c, 1 );
    return r;
}