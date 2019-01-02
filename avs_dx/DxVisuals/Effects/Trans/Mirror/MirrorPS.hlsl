#include "VisCommon.fx"
#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
#endif
Texture2D<float4> texPrevFrame : register(BIND_PREV_FRAME);

cbuffer MirrorCb : register(BIND_CB)
{
	// -1.0 = copy end to begin, 0 = do nothing, +1.0 = copy begin to end
    float2 amounts : packoffset(c0.x);

    bool transitionFlag : packoffset(c0.z);
}

#define halfSize ( AVS_RENDER_SIZE * 0.5 )

// pos is absolute position relative to the center
inline float3 read( float2 pos )
{
    return texPrevFrame.Load( int3( int2( pos + halfSize ), 0 ) ).rgb;
}

inline float3 mirrorSimple( float2 pos )
{
    const int2 thisSegment = sign( pos - 0.1 ); // { -1,-1 } for the top-left quadrant of the image
    const int2 amountSigns = sign( amounts );
    pos = ( thisSegment == amountSigns ) ? -pos : pos; // Apparently, HLSL has vector `operator?`: the `==` returns bool2 here.
    return read( pos );
}

inline float lengthSquared( float2 v )
{
    return dot( v, v );
}

inline float3 mirrorWithTransitions( float2 pos )
{
    const int2 thisSegment = sign( pos - 0.1 ); // { -1,-1 } for the top-left quadrant of the image

    const float2 a1 = floor( amounts );
    const float2 a2 = ceil( amounts );

    const int2 s1 = sign( a1 );
    const int2 s2 = sign( a2 );
    float3 res;
    if( all( s1 == s2 ) )
    {
		// Happens when this quadrant doesn't need to do anything, i.e. it's the source rectangle.
        res = mirrorSimple( pos );
    }
    else if( any( s1 == s2 ) )
    {
        const float2 p1 = ( thisSegment == s1 ) ? -pos : pos;
        const float2 p2 = ( thisSegment == s2 ) ? -pos : pos;

        const float2 val2 = amounts - a1;
        const float val = max( val2.x, val2.y );
        res = lerp( read( p1 ), read( p2 ), val );
    }
    else
    {
		// Worst-case scenario, have to read from 4 pixels
        const float2 uv1 = ( thisSegment == s1 ) ? -pos : pos;
        const float2 uv2 = ( thisSegment == s2 ) ? -pos : pos;

        const float3 pixels[ 4 ] =
        {
            read( uv1 ),
			read( float2( uv2.x, uv1.y ) ),
			read( float2( uv1.x, uv2.y ) ),
			read( uv2 ),
        };

        const float2 c1 = amounts - a1;
        const float2 c2 = a1 + 1 - amounts;
        res = pixels[ 0 ] * c2.x * c2.y + pixels[ 1 ] * c1.x * c2.y +
			pixels[ 2 ] * c2.x * c1.y + pixels[ 3 ] * c1.x * c1.y;
    }
    return res;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float2 pos = screenSpace.xy - halfSize;

    float3 res;
    if( !transitionFlag )
        res = mirrorSimple( pos );
    else
        res = mirrorWithTransitions( pos );
    return float4( res, 1 );
}