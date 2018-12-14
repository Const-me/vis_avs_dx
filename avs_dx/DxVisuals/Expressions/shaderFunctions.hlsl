#include "../Effects/VisCommon.fx"

// This file contains global functions available to the expression engine.
// This file gets parsed in design-time into individual functions: this approach saves compilation time, also allows to detect undefined functions earlier, without even calling HLSL compiler.

inline uint gettime()
{
    return getTickCount;
}

// returns spectrum data centered at 'band', (0..1), sampled 'width' (0..1) wide.
// 'channel' can be: 0=center, 1=left, 2=right. return value is (-1..1)
inline float getspec( float band, float width, uint channel )
{
    const float vVals[ 3 ] = { 0.25, 0.125, 0.375 };
    const float v = vVals[ channel ];

    width *= 0.5;
    float2 rangeFloat = saturate( float2( band - width, band + width ) );
    uint2 rangeInt = (uint2) ( rangeFloat * 576.0 );
    float res = 0;
    for( uint x = rangeInt.x; x < rangeInt.y; x += 2 )
    {
        float u = (float) x * ( 1.0 / 576.0 );
        res += texVisDataU8.Sample( sampleBilinear, float2( u, v ) );
    }
    const uint count = ( rangeInt.y - rangeInt.x ) << 1;
    return res / (float) count;
}

// returns waveform data centered at 'band', (0..1), sampled 'width' (0..1) wide.
// 'channel' can be: 0=center, 1=left, 2=right. return value is (-1..1)
inline float getosc( float band, float width, uint channel )
{
    const float vVals[ 3 ] = { 0.75, 0.625, 0.875 };
    const float v = vVals[ channel ];

    width *= 0.5;
    float2 rangeFloat = saturate( float2( band - width, band + width ) );
    uint2 rangeInt = (uint2) ( rangeFloat * 576.0 );
    float res = 0;
    for( uint x = rangeInt.x; x < rangeInt.y; x += 2 )
    {
        float u = (float) x * ( 1.0 / 576.0 );
        res += texVisDataS8.Sample( sampleBilinear, float2( u, v ) );
    }
    const uint count = ( rangeInt.y - rangeInt.x ) << 1;
    return res / (float) count;
}

[numthreads( 1, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
}