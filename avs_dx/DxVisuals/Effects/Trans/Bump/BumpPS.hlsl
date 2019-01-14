#include "VisCommon.hlsli"
#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
static const bool invertDepth = false;
static const float pxRadius = 200.0f;
#endif
Texture2D<float4> texPrevFrame : register(t3);

inline float depthOf( int2 pos )
{
    float3 rgb = texPrevFrame.Load( int3( pos, 0 ) ).rgb;
    float res = max( max( rgb.r, rgb.g ), rgb.b );
    return invertDepth ? 1.0 - res : res;
}

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
    const float len = length( offset );

    if( len > pxRadius )
        return 0; // Saves a lot of texture bandwidth

    value *= smoothstep( pxRadius, 0, len );

	// Debug code below, only adjust brightness
    // return value;

    const int2 px = (int2) screenSpace.xy;

    float2 norm;
    norm.x = depthOf( px + int2( -1, 0 ) ) - depthOf( px + int2( +1, 0 ) );
    norm.y = depthOf( px + int2( 0, -1 ) ) - depthOf( px + int2( 0, +1 ) );
    norm *= 0.5;
    float light = 1 + norm.x + norm.y;
    
    return saturate( light * value );
}