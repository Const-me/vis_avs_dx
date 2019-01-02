#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

cbuffer ColorFadeCb : register(BIND_CB)
{
    float3 ft[ 4 ];
}

inline uint calcTab( float3 color )
{
	// Return what was calculated in unsigned char c_tab[ 512 ][ 512 ];
    const float xp = color.g - color.b;
    const float yp = color.b - color.r;
    if( xp > 0 /* g-b > 0, or g > b */ && xp > -yp /* g-b > r-b, or g > r */ )
        return 0;
    else if( yp < 0 /* b-r < 0 or r > b */ && xp < -yp /* g-b < r-b, or g < r */ )
        return 1;
    else if( xp < 0 && yp > 0 )
        return 2;
    else
        return 3;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float3 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).rgb;
    const uint p = calcTab( color );
    const float3 clipped = saturate( color + ft[ p ] );
    return float4( clipped, 1 );
}