#ifndef AVS_SHADER
// A geometry shader that expands lineadj into triangles.
static const float sizeInPixels = 4;
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif
#include "Polyline.hlsli"

// 1.0: always miter, -1.0: never miter, 0.75: default
#define MITER_LIMIT 0.75

inline float2 pixelFromClip( float2 cs )
{
    const float2 mul = AVS_RENDER_SIZE * 0.5f;
    return cs * mul;
}

inline float2 pixelFromClip( float4 cs )
{
    return pixelFromClip( cs.xy / cs.w );
}

inline float2 clipFromPixel( float2 px )
{
    const float2 mul = float2( 2, 2 ) / AVS_RENDER_SIZE;
    return px * mul;
}

inline float2 screenFromPixel( float2 px )
{
    const float2 halfSize = 0.5 * AVS_RENDER_SIZE;
    return px * float2( 1, -1 ) + halfSize;
}

inline float2 rotate90( float2 v )
{
    return float2( -v.y, v.x );
}

inline bool notZero( float2 v )
{
    return dot( v, v ) > 1E-4; // Length > 0.01
}

inline float4 makePosition( float2 px )
{
    return float4( clipFromPixel( px ), 0.5, 1 );
}

void addTwoVerts( inout TriangleStream<sVertex2> output, sVertex2 gso, float2 base, float2 add, float4 col )
{
    gso.color = col;

    gso.position = makePosition( base - add );
    // gso.texCoord = -1;
    output.Append( gso );

    gso.position = makePosition( base + add );
    // gso.texCoord = +1;
    output.Append( gso );
}

[maxvertexcount( 7 )]
void main( lineadj sVertex1 lineVerts[ 4 ], inout TriangleStream<sVertex2> output )
{
	// Two segment points in pixel space
    const float2 p1 = pixelFromClip( lineVerts[ 1 ].pos );
    const float2 p2 = pixelFromClip( lineVerts[ 2 ].pos );

	// Direction & normal
    const float2 v1 = normalize( p2 - p1 );
    const float2 n1 = rotate90( v1 );

    float2 miter_a = n1;
    float length_a = sizeInPixels;

    float2 miter_b = n1;
    float length_b = sizeInPixels;

	// Previous and next points pixels, in xy and zw fields
    const float4 prevNextPoints = float4( pixelFromClip( lineVerts[ 0 ].pos ), pixelFromClip( lineVerts[ 3 ].pos ) );

    sVertex2 gso;
    gso.segmentPoints = float4( screenFromPixel( p1 ), screenFromPixel( p2 ) );

    if( notZero( p1 - prevNextPoints.xy ) )
    {
        // Previous point != p1
        const float2 v0 = normalize( p1 - prevNextPoints.xy );
        const float2 n0 = rotate90( v0 );

        if( dot( v0, v1 ) >= -MITER_LIMIT )
        {
            miter_a = normalize( n0 + n1 ); // miter at start of current segment
            length_a = sizeInPixels / dot( miter_a, n1 );
        }
        else
        {
            gso.color = lineVerts[ 1 ].color;

            if( dot( v0, n1 ) > 0 )
            {
                // gso.texCoord = -1;
                gso.position = makePosition( p1 + sizeInPixels * n0 );
                output.Append( gso );

                // gso.texCoord = -1;
                gso.position = makePosition( p1 + sizeInPixels * n1 );
                output.Append( gso );

                // gso.texCoord = 0;
                gso.position = makePosition( p1 );
                output.Append( gso );

                output.RestartStrip();
            }
            else
            {
                // gso.texCoord = 1;
                gso.position = makePosition( p1 - sizeInPixels * n1 );
                output.Append( gso );

                // gso.texCoord = 1;
                gso.position = makePosition( p1 - sizeInPixels * n0 );
                output.Append( gso );

                // gso.texCoord = 0;
                gso.position = makePosition( p1 );
                output.Append( gso );

                output.RestartStrip();;
            }
        }
    }

    if( notZero( prevNextPoints.zw - p2 ) )
    {
        // Next point != p2
        const float2 v2 = normalize( prevNextPoints.zw - p2 );
        if( dot( v1, v2 ) >= -MITER_LIMIT )
        {
            const float2 n2 = rotate90( v2 );
            miter_b = normalize( n1 + n2 );
            length_b = sizeInPixels / dot( miter_b, n1 );
        }
    }

    addTwoVerts( output, gso, p1, length_a * miter_a, lineVerts[ 1 ].color );
    addTwoVerts( output, gso, p2, length_b * miter_b, lineVerts[ 2 ].color );
}