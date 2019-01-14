Texture2D<float4> texSource : register(t3);
SamplerState sampleBilinear : register(s0);

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    float4 src = texSource.Sample( sampleBilinear, tc );
    src.a = max( max( src.r, src.g ), src.b );
    return src;
}