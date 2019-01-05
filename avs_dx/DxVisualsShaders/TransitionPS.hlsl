Texture2D<float4> texSource : register(t0);
Texture2D<float4> texDest : register(t1);
SamplerState sampleBilinear : register(s0);
SamplerState sampleNearest : register(s1);

cbuffer TransitionConstants : register(b0)
{
    float sintrans : packoffset(c0.x);
    uint curtrans : packoffset(c0.y);
    uint mask : packoffset(c0.z);
}

inline float4 crossfade( float2 tc )
{
    float4 a = texSource.SampleLevel( sampleBilinear, tc, 0 );
    float4 b = texDest.SampleLevel( sampleBilinear, tc, 0 );
    return lerp( a, b, sintrans );
}

inline float4 push( float2 tc, float x, float y )
{
    const float2 movePos = tc + float2( x, y ) * ( 1.0 - sintrans );
    if( all( movePos >= 0 && movePos <= 1 ) )
        return texDest.SampleLevel( sampleBilinear, movePos, 0 );
    else
        return texSource.SampleLevel( sampleBilinear, movePos, 0 );
}

inline float4 randomBlocks( float2 tc )
{
    uint2 i2 = (uint2) ( floor( tc * 3 ) );
    i2 = min( i2, 2 );
    uint i = i2.y * 3 + i2.x;

    if( 0 == ( mask & ( 1 << i ) ) )
        return texSource.SampleLevel( sampleBilinear, tc, 0 );
    else
        return texDest.SampleLevel( sampleBilinear, tc, 0 );
}

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    switch( curtrans )
    {
        case 1: // Crossfade
            return crossfade( tc );
        case 2: // Left to right push
            return push( tc, 1, 0 );
        case 3: // Right to left push
            return push( tc, -1, 0 );
        case 4: // Top to bottom push
            return push( tc, 0, -1 );
        case 5: // Bottom to Top push
            return push( tc, 0, 1 );
        case 6: // 9 random blocks
            return randomBlocks( tc );
    }

	// TODO: implement the rest of them
    return crossfade( tc );
}