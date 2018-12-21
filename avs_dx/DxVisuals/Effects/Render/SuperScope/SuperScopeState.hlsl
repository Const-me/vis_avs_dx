// <global>
inline float3 unpackBgr( uint bgr )
{
    uint3 iv = uint3( bgr >> 16, bgr >> 8, bgr );
    iv &= uint3( 0xFF, 0xFF, 0xFF );
    return ( (float3) iv ) * ( 1.0 / 255.0 );
}
// </global>

#define STATE_OFFSET 7
#define num_colors 3
#define COLOR_VALUES 0xFF0000, 0xFF00, 0xFF
#define INIT_STATE 0
// <macro-vector type="int" name="colors" size="num_colors" values="COLOR_VALUES" />

RWByteAddressBuffer effectStates : register(u0);

[numthreads( 1, 1, 1 )]
void main()
{
    const uint colors[ num_colors ] = { COLOR_VALUES };
#if INIT_STATE
	effectStates.Store( STATE_OFFSET, 0 );
	effectStates.Store3( STATE_OFFSET + 4, asuint( unpackBgr( colors[ 0 ] ) ) );
#else
    uint color_pos = effectStates.Load( STATE_OFFSET );
    color_pos++;
    if( color_pos >= num_colors * 64 )
        color_pos = 0;
    effectStates.Store( STATE_OFFSET, color_pos );
	
    uint p = color_pos / 64;
    uint r = color_pos & 63;
    uint c1, c2;
    uint r1, r2, r3;
    c1 = colors[ p ];
    if( p + 1 < num_colors )
        c2 = colors[ p + 1 ];
    else
        c2 = colors[ 0 ];

    const float3 res = lerp( unpackBgr( c1 ), unpackBgr( c2 ), (float) r * ( 1.0 / 64.0 ) );
    effectStates.Store3( STATE_OFFSET + 4, asuint( res ) );
#endif
}