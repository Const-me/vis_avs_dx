#include "FrameGlobalData.hlsli"
#ifndef AVS_SHADER
#define AVS_PIXEL_PARTICLES 0
#endif

inline float4 alphaBlend( float3 rgb, float alpha )
{
    float4 result;
    if( lineModeAllowAlpha )
        result = float4( rgb * alpha, alpha );
    else
    {
        if( alpha >= 0.875 )
            result = float4( rgb, 1 );
        else
        {
            result = 0;
            discard;
        }
    }
    return result;
}


float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
#if AVS_PIXEL_PARTICLES
    return float4( color.rgb, 1 );
#else
    const float len2 = dot( tc, tc );
    const float alpha = exp( -4 * len2 );
    return alphaBlend( color.rgb, alpha );
#endif
}