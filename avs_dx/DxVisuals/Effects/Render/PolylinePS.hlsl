#include "LinesUtils.hlsli"
#include "Polyline.hlsli"

#ifndef AVS_SHADER
static const float sizeInPixels = 4;
#define AVS_PIXEL_PARTICLES 0
#endif

// Distance between point and segment
float distanceSquared( float2 pt, float2 s1, float2 s2 )
{
    const float2 segDir = s2 - s1;

    const float segLengthSqr = lengthSquared( segDir );
    float result;
    if( segLengthSqr < 1E-4 )
        result = lengthSquared( pt - s1 );	// Empty segment
	else
    {
        float k = dot( pt - s1, segDir ) / segLengthSqr; // |p-p1| * |p2-p1| * cos(a) / |p2-p1|^2 == |p-p1| * cos(a) / |p2-p1|
        k = saturate( k );
        const float2 proj = s2 * k + s1 * ( 1.0 - k );
        result = lengthSquared( pt - proj );
    }
    return result;
}

float4 main( sVertex2 vert ) : SV_Target0
{
    const float dsq = distanceSquared( vert.position.xy, vert.segmentPoints.xy, vert.segmentPoints.zw );
    const float sizeSq = sizeInPixels * sizeInPixels;
    if( dsq >= sizeSq )
    {
        discard;
        return float4( 0, 0, 0, 0 );
    }

    const float mul = -4.158883083 / sizeSq;
    const float alpha = exp( mul * dsq );

#if AVS_PIXEL_PARTICLES
	return alphaCutoffBlend( vert.color.rgb, alpha );
#else
    return alphaBlend( vert.color.rgb, alpha );
#endif
}