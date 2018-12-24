#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#endif
Texture2D<float4> texPrevFrame : register( BIND_PREV_FRAME );
SamplerState sampleBilinear : register(s0);

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    return texPrevFrame.Sample( sampleBilinear, tc );
}