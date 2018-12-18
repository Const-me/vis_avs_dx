#include "VisCommon.fx"
#ifndef AVS_SHADER
#define BIND_DOTS_POSITIONS u4
// <macro-resource name="bindDotsPosition" macro="BIND_DOTS_POSITIONS" type="uav" />
static const float2 y1y2 = float2( -0.5f, 0.5f );
static const float sampleV = 0.75f;
static const uint pointsCount = 768;
#endif
RWStructuredBuffer<float2> dots : register(BIND_DOTS_POSITIONS);

[numthreads( 256, 1, 1 )]
void main( in uint3 threadId : SV_DispatchThreadID )
{
    const uint id = threadId.x;
    if( id >= pointsCount )
        return;
    const float countInv = 1.0f / ( pointsCount - 1 );
    const float f = countInv * ( float )id;
    const bool isWaveform = ( sampleV > 0.5 );
    float val = sampleVisData( float2( f, sampleV ), isWaveform );
    if( isWaveform )
        val = ( val + 1 ) * 0.5;

    float2 pos;
    pos.x = lerp( -1, 1, f );
    pos.y = lerp( y1y2.x, y1y2.y, val );
    dots[ id ] = pos;
}