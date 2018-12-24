#ifndef AVS_SHADER
#define BIND_VIDEO_FRAME t1
// <macro-resource name="bindVideoFrame" macro="BIND_VIDEO_FRAME" />
#endif
SamplerState sampleBilinear : register(s0);
Texture2D<float4> texVideoFrame : register(BIND_VIDEO_FRAME);

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    return texVideoFrame.Sample( sampleBilinear, tc );
}