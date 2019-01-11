#include "VisCommon.hlsli"
#ifndef AVS_SHADER
static const uint levels = 1;
#endif
Texture2D<float4> texPrevFrame : register(t3);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const uint p2 = (uint) 1 << (uint) levels;
    const float mul = (float) ( p2 );
    const float div = 1.0 / (float) ( p2 - 1 );

    float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    color *= mul;
    color = floor( color );
    color = saturate( color * div );
    return float4( color, 1 );
}