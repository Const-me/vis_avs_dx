#include "VisCommon.hlsli"
#include "Polyline.hlsli"

#ifndef AVS_SHADER
static const uint pointsCount = 768;
static const float2 y1y2 = float2( -0.5f, 0.5f );
static const float sampleV = 0.75f;
#endif

sVertex1 main( uint id : SV_VertexID )
{
    if( 0 != id )
        id--;

    if( id >= pointsCount )
        id = pointsCount - 1;

    const float countInv = 1.0f / ( pointsCount - 1 );
    const float f = countInv * (float) id;
    const bool isWaveform = ( sampleV > 0.5 );
    float val = sampleVisData( float2( f, sampleV ), isWaveform );
    if( isWaveform )
        val = ( val + 1 ) * 0.5;

    float2 pos;
    pos.x = lerp( -1, 1, f );
    pos.y = lerp( y1y2.x, y1y2.y, val );

    sVertex1 v;
    v.pos = float4( pos, 0.5, 1 );
    v.color = float4( stateFloat3( 1 ), 1 );
    return v;
}