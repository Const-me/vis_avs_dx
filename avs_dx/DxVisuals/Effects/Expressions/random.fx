inline uint avs_rand_init( uint seed )
{
    seed = ( seed ^ 61 ) ^ ( seed >> 16 );
    seed *= 9;
    seed = seed ^ ( seed >> 4 );
    seed *= 0x27d4eb2d;
    seed = seed ^ ( seed >> 15 );
    return seed;
}

inline float avs_rand( inout uint rng_state, float maxVal )
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= ( rng_state << 13 );
    rng_state ^= ( rng_state >> 17 );
    rng_state ^= ( rng_state << 5 );

    const float res = float( rng_state ) * ( 1.0 / 4294967296.0 );
    return maxVal * res;
}