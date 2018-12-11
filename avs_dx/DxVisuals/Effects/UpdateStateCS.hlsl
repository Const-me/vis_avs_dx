#ifndef AVS_SHADER
#define SHADER_CODE
#endif

RWByteAddressBuffer effectStates : register(u0);

inline float3 unpackBgr( uint bgr )
{
    uint3 iv = uint3( bgr >> 16, bgr >> 8, bgr );
    iv &= uint3( 0xFF, 0xFF, 0xFF );
    return ( (float3) iv ) * ( 1.0 / 255.0 );
}

[numthreads( 1, 1, 1 )]
void main()
{
    uint currentFrame = effectStates.Load( 0 );
    currentFrame++;
    effectStates.Store( 0, currentFrame );

	SHADER_CODE
}