#include "VisCommon.hlsli"
#include "Polyline.hlsli"

#ifndef AVS_SHADER
static const float sampleV = 0.75f;
static const uint drawLines = 0;
#define SHADER_GLOBALS
#define SHADER_CODE
#endif

SHADER_GLOBALS

sVertex1 main( uint id : SV_VertexID )
{
    const float3 color = stateFloat3( 1 );
    const uint pointsCount = stateUint( 4 );

    if( drawLines )
    {
		// Lines are rendered with D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ, injecting duplicating vertices for start/end
		if( id > 0 )
            id--;
        if( id >= pointsCount )
            id = pointsCount - 1;
    }
    float i = (float) id / (float) ( pointsCount - 1 );
    float v = sampleVisData( float2( i, sampleV ), ( sampleV > 0.5 ) );

    float red = color.x;
    float green = color.y;
    float blue = color.z;
    float x = 0, y = 0, skip, linesize;

#if NEEDS_RNG
    uint rng_state = avs_rand_init( getRandomSeed( id ) );
#endif
    {
SHADER_CODE
    }

	// TODO: use skip / linesize values.

    sVertex1 vert;
    vert.pos = float4( x, -y, 0.5, 1 );
    vert.color = float4( red, green, blue, 1 );
    return vert;
}