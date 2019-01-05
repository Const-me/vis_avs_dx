#ifndef AVS_SHADER
#define BIND_SAMPLER s1
// <macro-resource name="bindSampler" macro="BIND_SAMPLER" type="sampler" />
#endif
Texture2D<float4> texPrevFrame : register(t3);
SamplerState ss : register(BIND_SAMPLER);

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    return texPrevFrame.Sample( ss, tc );
}