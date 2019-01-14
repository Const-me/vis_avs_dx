#include "VisCommon.hlsli"
#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif
Texture2D<float4> texPrevFrame : register(t3);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	// xy from 0 (left) to 1 (right). z is bump intensity, ranges from 0 (flat) to 1 (max specified bump, default)
    float4 stateData = stateFloat4( 1 );

    const float fixedValue = stateData.x;
    const float2 xy = stateData.yz;
    const float calculatedValue = stateData.w;
    float value = fixedValue * saturate( calculatedValue );

    const float2 pxCenter = xy * AVS_RENDER_SIZE;
    const float2 offset = screenSpace.xy - pxCenter;

    const float expMul = -0.5E-4;
    value *= exp( expMul * dot( offset, offset ) );

    float3 rgb = texPrevFrame.Load( int3( (int2) screenSpace.xy, 0 ) ).rgb;
	// Debug code below, only adjust brightness
    return value * 0.5 + 0.5;
}