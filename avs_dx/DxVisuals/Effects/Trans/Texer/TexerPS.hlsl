Texture2D<float4> texSource : register(t3);
SamplerState sampleBilinear : register(s0);

float4 main( float2 tc : TEXCOORD0, float3 color : COLOR0 ) : SV_Target
{
    color *= texSource.Sample( sampleBilinear, tc ).rgb;
    return float4( color, 1 );
}