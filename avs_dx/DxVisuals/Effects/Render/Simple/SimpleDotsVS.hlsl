#include "VisCommon.fx"

#ifndef AVS_SHADER
static const float2 y1y2 = float2( -0.5f, 0.5f );
static const float sampleV = 0.75f;
static const uint pointsCount = 768;
#endif

struct SimpleVertex
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

SimpleVertex main( uint id : SV_VertexID )
{
    const float countInv = 1.0f / ( pointsCount - 1 );
    const float f = countInv * (float) id;
    const bool isWaveform = ( sampleV > 0.5 );
    float val = sampleVisData( float2( f, sampleV ), isWaveform );
    if( isWaveform )
        val = ( val + 1 ) * 0.5;

    float2 pos;
    pos.x = lerp( -1, 1, f );
    pos.y = lerp( y1y2.x, y1y2.y, val );

    SimpleVertex r;
    r.pos = float4( pos, 0.5, 1 );
    r.color = float4( stateFloat3( 1 ), 1 );
    return r;
}