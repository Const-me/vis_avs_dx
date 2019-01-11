#include "VisCommon.hlsli"
#include "Fountain.hlsli"
#ifndef AVS_SHADER
#define BIND_PARTICLES t4
// <macro-resource name="bindParticles" macro="BIND_PARTICLES" />

#define BIND_CB b4
// <macro-resource name="bindCBuffer" macro="BIND_CB" type="cbuffer" />
#endif

StructuredBuffer<FountainPoint> particles : register(BIND_PARTICLES);

cbuffer FountainCBuffer : register(BIND_CB)
{
    matrix worldViewProj : packoffset(c0);
}

struct SimpleVertex
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

SimpleVertex main( uint id : SV_VertexID )
{
    float4 pos = float4( particles[ id ].position, 1 );
    SimpleVertex r;
    r.pos = mul( pos, worldViewProj );
    r.color = float4( particles[ id ].color, 1 );
    return r;
}