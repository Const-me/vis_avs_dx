#include "FrameGlobalData.fx"

// Texture of size 576 * 4, signed bytes i.e. DXGI_FORMAT_R8_SNORM
// First two lines are spectrum, left/right channel, second two lines are wave.
// Y texture coordinates: 0.125 left channel spectrum, 0.25 center channel spectrum, 0.375 right channel spectrum, 0.625 left channel waveform, 0.75 center channel waveform, 0.875 right channel waveform
Texture2D<float> texVisData : register(t0);

ByteAddressBuffer effectStates : register(t1);

SamplerState sampleBilinear : register(s0);

inline float sampleVisData(float2 location)
{
    return texVisData.Sample(sampleBilinear, location);
}

#ifndef AVS_SHADER
#define STATE_OFFSET 0
#endif

inline uint stateUint(int i)
{
    return effectStates.Load( STATE_OFFSET + i * 4 );
}
inline float stateFloat( int i )
{
    return asfloat( effectStates.Load( STATE_OFFSET + i * 4 ) );
}
inline float4 stateFloat4( int i )
{
    return asfloat( effectStates.Load4( STATE_OFFSET + i * 4 ) );
}

inline uint wang_hash( uint seed )
{
    seed = ( seed ^ 61 ) ^ ( seed >> 16 );
    seed *= 9;
    seed = seed ^ ( seed >> 4 );
    seed *= 0x27d4eb2d;
    seed = seed ^ ( seed >> 15 );
    return seed;
}

inline uint srand( uint seed )
{
    return wang_hash( seed ^ getTickCount );
}

inline uint rand_xorshift( inout uint rng_state )
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= ( rng_state << 13 );
    rng_state ^= ( rng_state >> 17 );
    rng_state ^= ( rng_state << 5 );
    return rng_state;
}

inline float randf( inout uint rng_state )
{
    return float( rand_xorshift( rng_state ) ) * ( 1.0 / 4294967296.0 );
}

// Random 2D position in clip space i.e. within [ -1 .. +1 ] interval
inline float2 randomPos( inout uint rng_state )
{
    float2 res = float2( randf( rng_state ), randf( rng_state ) );
    return res * 2 - float2( 1, 1 );
}