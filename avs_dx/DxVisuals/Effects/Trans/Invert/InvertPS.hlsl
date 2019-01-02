#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    return float4( 1.0 - color, 1 );
}