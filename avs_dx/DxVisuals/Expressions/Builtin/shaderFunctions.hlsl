#include "../Effects/VisCommon.fx"

// This file contains global functions available to the expression engine.
// This file gets parsed in design-time into individual functions: this approach saves compilation time, also allows to detect undefined functions earlier, without even calling HLSL compiler.

inline double gettime( float x )
{
    return (double) getTickCount * double( 1E-3 );
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

// The getspec / getosc functions below deliberately sample data between pixels to reduce count of texture lookups by 50%
// http://www.asawicki.info/news_1516_half-pixel_offset_in_directx_11.html

// returns spectrum data centered at 'band', (0..1), sampled 'width' (0..1) wide.
// 'channel' can be: 0=center, 1=left, 2=right. return value is (0..1)
inline float getspec( float band, float width, uint channel )
{
    const float vVals[ 3 ] = { 0.25, 0.125, 0.375 };
    const float v = vVals[ channel ];
    const float mul = 1.0 / 576.0;
    if( width < 2.0 * mul )
        return texVisDataU8.SampleLevel( sampleBilinear, float2( band * mul, v ), 0 );

    width *= 0.5;
    float2 rangeFloat = saturate( float2( band - width, band + width ) );
    uint2 rangeInt = (uint2) ( rangeFloat * 576.0 );
    float res = 0;
    for( uint x = rangeInt.x; x < rangeInt.y; x += 2 )
    {
        float u = (float) x * mul;
        res += texVisDataU8.SampleLevel( sampleBilinear, float2( u, v ), 0 );
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
    const float mul = 1.0 / 576.0;
    if( width < 2.0 * mul )
        return texVisDataS8.SampleLevel( sampleBilinear, float2( band * mul, v ), 0 );

    width *= 0.5;
    float2 rangeFloat = saturate( float2( band - width, band + width ) );
    uint2 rangeInt = (uint2) ( rangeFloat * 576.0 );
    float res = 0;
    for( uint x = rangeInt.x; x < rangeInt.y; x += 2 )
    {
        float u = (float) x * mul;
        res += texVisDataS8.SampleLevel( sampleBilinear, float2( u, v ), 0 );
    }
    const uint count = ( rangeInt.y - rangeInt.x ) * 2;
    return res / (float) count;
}

[numthreads( 1, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
}