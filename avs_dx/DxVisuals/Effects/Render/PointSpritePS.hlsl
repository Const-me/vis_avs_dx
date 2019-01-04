#ifndef AVS_SHADER
#define AVS_PIXEL_PARTICLES 0
#endif

float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
#if AVS_PIXEL_PARTICLES
    return color;
#else
    const float len2 = dot( tc, tc );
    return color * exp( -4 * len2 );
#endif
}