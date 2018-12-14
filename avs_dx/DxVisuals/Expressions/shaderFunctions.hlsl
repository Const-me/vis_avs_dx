#include "../Effects/VisCommon.fx"

// This file contains global functions available to the expression engine.
// This file gets parsed in design-time into individual functions: this approach saves compilation time, also allows to detect undefined functions earlier, without even calling HLSL compiler.

inline uint gettime( float x )
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

inline float sqr( float x )
{
    return x * x;
}

inline float invsqrt( float x )
{
    return rsqrt( x );
}

inline float above( float a, float b )
{
    return a > b ? 1 : 0;
}

inline float below( float a, float b )
{
    return a < b ? 1 : 0;
}

inline float sigmoid( float x, float constraint )
{
    const float t = 1.0 + exp( -x * constraint );
    if( 0.0 == t )
        return 0;
    return 1.0 / t;
}

[numthreads( 1, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
}