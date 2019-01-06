#ifndef AVS_SHADER
static const uint sourceChannel = 5;
#define BIND_MAP t6
// <macro-resource name="bindMap" macro="BIND_MAP" />
static const uint blend = 1;
static const float blendVal = 1.0f;
#endif
Texture2D<float4> texPrevFrame : register(t3);
Texture1D<float4> texMap : register(BIND_MAP);
SamplerState sampleBilinear : register(s0);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float4 d = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );

    float x = 0;
    switch( sourceChannel )
    {
        case 0:
            x = d.r;
            break;
        case 1:
            x = d.g;
            break;
        case 2:
            x = d.b;
            break;
        case 3:
            x = ( d.r + d.g + d.b ) / 2;
            break;
        case 4:
            x = max( max( d.r, d.g ), d.b );
            break;
        case 5:
            x = ( d.r + d.g + d.b ) / 3;
            break;
    }

    const float4 s = texMap.SampleLevel( sampleBilinear, x, 0 );

	// `blend` will become macro in runtime, i.e. these branches are free.
    switch( blend )
    {
        case 0: // Replace
            return s;
        case 1: // Additive
            return saturate( s + d );
        case 2: // Maximum
            return max( s, d );
        case 3: // Minimum
            return min( s, d );
        case 4: // 50/50
            return ( s + d ) * 0.5;
        case 5: // Subtractive 1
            return saturate( s - d );
        case 6: // Subtractive 2
            return saturate( d - s );
        case 7: // Multiply
            return s * d;
        case 8: // XOR
            {
				// We use DXGI_FORMAT_R10G10B10A2_UNORM for render targets, that's why 1023 instead of the traditional 255.
                const uint3 su = (uint3) ( s.xyz * 1023.0 );
                const uint3 du = (uint3) ( d.xyz * 1023.0 );
                const uint3 x = su ^ du;
                return float4( ( (float3) x ) * ( 1.0 / 1023.0 ), 1 );
            }
        case 9: // Adjustable
            return s * blendVal + d * ( 1.0 - blendVal );
    }
    return float4( 0, 0, 0, 0 );
}