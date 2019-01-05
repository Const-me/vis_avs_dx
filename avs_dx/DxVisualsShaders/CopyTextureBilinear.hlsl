Texture2D<float4> texSource : register(t3);
SamplerState sampleBilinear : register(s0);

float4 main( float2 tc: TEXCOORD0 ) : SV_Target
{
    return texSource.Sample( sampleBilinear, tc );
}