#include "VisCommon.hlsli"
#ifndef AVS_SHADER
static const float3 values = float3( 1, 1, 1 );
static const bool exclude = false;
static const float3 excludeColor = float3( 1, 1, 1 );
static const float excludeDistance = 0.125f;
#endif
Texture2D<float4> texPrevFrame : register(t3);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;

    if( exclude )
    {
        float3 dist = abs( color - excludeColor );
        if( all( dist <= excludeDistance ) )
            return float4( color, 1 );
    }
    return float4( color * values, 1 );
}