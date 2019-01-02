#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
static const uint enabled = 1;
static const float3 color_clip = float3( 0, 0, 0 );
static const float3 color_clip_out = float3( 0, 0, 0 );
static const float color_dist = 0.1f;
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

inline float lengthSquared( float3 v )
{
    return dot( v, v );
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    switch( enabled )
    {
        case 1:
            if( all( color <= color_clip ) )
                color = color_clip_out;
            break;
        case 2:
            if( all( color >= color_clip ) )
                color = color_clip_out;
            break;
        case 3:
            if( lengthSquared( color_clip - color ) <= 4 * color_dist * color_dist )
                color = color_clip_out;
            break;
    }
    return float4( color, 1 );
}