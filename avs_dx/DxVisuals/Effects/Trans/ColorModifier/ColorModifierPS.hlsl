#include "VisCommon.fx"
#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE
#endif
Texture2D<float4> texPrevFrame : register(t3);

SHADER_GLOBALS

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	const float4 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
	float red = color.x;
	float green = color.y;
	float blue = color.z;
#if NEEDS_RNG
    uint rng_state = avs_rand_init( getRandomSeed( screenSpace.xy ) );
#endif

	SHADER_CODE
	return float4( red, green, blue, 1 );
}