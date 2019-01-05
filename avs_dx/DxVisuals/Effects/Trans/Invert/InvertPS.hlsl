#include "VisCommon.fx"
Texture2D<float4> texPrevFrame : register(t3);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    return float4( 1.0 - color, 1 );
}