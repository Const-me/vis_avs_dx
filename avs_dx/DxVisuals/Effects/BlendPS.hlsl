// AVS implements weird blend modes that aren't supported even on D3D 11.1 with D3D11_LOGIC_OP. This pixel shader implements such blending.
#ifndef AVS_SHADER
#define BIND_SOURCE t1
#define BIND_DEST t2
// <macro-resource name="source" macro="BIND_SOURCE" />
// <macro-resource name="dest" macro="BIND_DEST" />
static const uint blend = 0;
static const float blendVal = 1.0f;
#endif
Texture2D<float4> texSource : register(BIND_SOURCE);
Texture2D<float4> texDest : register(BIND_DEST);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const int3 pos = int3( int2( screenSpace.xy ), 0 ); // The last component specifies the mipmap level.
    const float4 s = texSource.Load( pos );
    const float4 d = texDest.Load( pos );

	// `blend` will become macro in runtime, i.e. these branches are free. And hopefully, HLSL compiler will even optimize away unused texture loads for cases 0 and 1.
    switch( blend )
    {
        case 0: // Ignore
            return d;
        case 1: // Replace
            return s;
        case 2: // 50/50
            return ( s + d ) * 0.5;
        case 3: // Maximum
            return max( s, d );
        case 4: // Additive
            return saturate( s + d );
        case 5: // Subtractive 1
            return saturate( s - d );
        case 6: // Subtractive 2
            return saturate( d - s );
        case 7: // Every other line
            return ( 0 == ( (uint) pos.y & 1 ) ) ? s : d;
        case 8: // Every other pixel
            {
                const uint2 u = (uint2) pos.xy;
                return ( ( u.x ^ u.y ) & 1 ) ? s : d;
            }
        case 9: // XOR
            {
                const uint3 su = (uint3) ( s.xyz * 255.0 );
                const uint3 du = (uint3) ( d.xyz * 255.0 );
                const uint3 x = su ^ du;
                return float4( ( (float3) x ) * ( 1.0 / 255.0 ), 1 );
            }
        case 10: // Adjustable
            return s * blendVal + d * ( 1.0 - blendVal );
        case 11: // Multiply
            return s * d;
		// 12 is "Buffer", should be handled outside of this shader
        case 13: // Minimum
            return min( s, d );
    }
    return float4( 0, 0, 0, 0 );
}