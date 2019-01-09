#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
static const uint count = 3;
static const bool separateRgb = true;
#endif
Texture2D<float4> texPrevFrame : register(t3);
SamplerState sampleBilinear : register(s0);

cbuffer InterferenceCb : register(BIND_CB)
{
    float alpha : packoffset(c0.x);
    float2 offsets[ 8 ] : packoffset(c1);
}

inline float3 read( float2 px )
{
    const float2 posMul = float2( 1, 1 ) / AVS_RENDER_SIZE;
    return texPrevFrame.SampleLevel( sampleBilinear, px * posMul, 0 ).rgb;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float2 pos = screenSpace.xy;
    float3 res = 0;
    if( separateRgb )
    {
		[unroll]
        for( uint i = 0; i < count; i += 3 )
        {
            float2 p = pos + offsets[ i ];
            res.r += alpha * read( p ).r;
            p = pos + offsets[ i + 1 ];
            res.g += alpha * read( p ).g;
            p = pos + offsets[ i + 2 ];
            res.b += alpha * read( p ).b;
        }
    }
    else
    {
		[unroll]
        for( uint i = 0; i < count; i++ )
        {
            float2 p = pos + offsets[ i ];
            res += alpha * read( p );
        }
    }
    return float4( saturate( res ), 1 );
}