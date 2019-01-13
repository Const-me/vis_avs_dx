#include "VisCommon.hlsli"
#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif

struct sOutput
{
	// Z component holds per-vertex alpha.
    float3 tc : TEXCOORD0;
    float4 pos : SV_Position;
};

sOutput main( uint id : SV_VertexID )
{
    const float3 stateData = stateFloat3( 0 );

	// https://web.archive.org/web/20140719063725/http://www.altdev.co/2011/08/08/interesting-vertex-shader-trick/
    sOutput v;
    v.tc.xy = float2( ( id << 1 ) & 2, id & 2 );
    v.tc.z = stateData.z;
    v.pos = float4( v.tc.xy * float2( 2, -2 ) + float2( -1, 1 ), 0.5, 1 );

	// Shifting tex.coords instead of vertices because free wrapping by texture units.
    const float2 shiftAmount = stateData.xy * ( 1.0 / AVS_RENDER_SIZE );
    v.tc.xy += shiftAmount;

    return v;
}