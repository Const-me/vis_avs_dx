Texture2D<float4> texSource : register(t15);
SamplerState sampleBilinear : register(s0);
#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float2 sizeInv = float2( 1, 1 ) / AVS_RENDER_SIZE;
    return texSource.Sample( sampleBilinear, sizeInv * screenSpace.xy );
}