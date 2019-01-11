#include "VisCommon.hlsli"
#ifndef AVS_SHADER
static const float3 color = float3( 0, 0, 0 );
#endif

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const uint2 pos = (uint2) screenSpace.xy;
    const uint2 size = stateUint2( 0 );

    if( size.x == 0 && size.y == 0 )
    {
        discard;
        return 0;
    }
    if( size.x == 0 )
    {
		// Horizontal stripes
        const uint y = pos.y / size.y;
        if( 0 == ( y & 1 ) )
            return float4( color, 1 );
        discard;
        return 0;
    }
    if( size.y == 0 )
    {
		// Vertical stripes
        const uint x = pos.x / size.x;
        if( 0 == ( x & 1 ) )
            return float4( color, 1 );
        discard;
        return 0;
    }

	// Grid
    const uint2 xy = pos / size;
    if( 0 == ( xy.x & xy.y & 1 ) )
        return float4( color, 1 );
    discard;
    return 0;
}