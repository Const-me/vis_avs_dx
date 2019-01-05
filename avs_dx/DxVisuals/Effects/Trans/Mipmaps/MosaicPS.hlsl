#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#define BIND_SAMPLER s1
// <macro-resource name="bindSampler" macro="BIND_SAMPLER" type="sampler" />
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
#endif

Texture2D<float4> texPrevFrame : register(t3);
SamplerState sampleBilinear : register(BIND_SAMPLER);

cbuffer MosaicCb : register(BIND_CB)
{
    float2 cellsCount: packoffset(c0.x);
    float LOD : packoffset(c0.z);
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    float2 uv = screenSpace.xy * ( 1.0 / AVS_RENDER_SIZE );
    uv = floor( uv * cellsCount ) / cellsCount;
    return texPrevFrame.SampleLevel( sampleBilinear, uv, LOD );
}