#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
static const uint mode = 0;
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

#define MD_XI    0
#define MD_X8    1
#define MD_X4    2
#define MD_X2    3
#define MD_X05   4
#define MD_X025  5
#define MD_X0125 6
#define MD_XS    7

inline float3 process( float3 rgb )
{
    const float eps = 1.0 / 512.0;
    switch( mode )
    {
        case MD_XI:
            return any( rgb > eps ) ? 1 : rgb;
        case MD_X8:
            return rgb * 8;
        case MD_X4:
            return rgb * 4;
        case MD_X2:
            return rgb * 2;
        case MD_X05:
            return rgb * 0.5;
        case MD_X025:
            return rgb * 0.25;
        case MD_X0125:
            return rgb * 0.125;
        case MD_XS:
            return any( rgb < 1.0 - eps ) ? 0 : rgb;
    }
    return rgb;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    return float4( process( color ), 1 );
}