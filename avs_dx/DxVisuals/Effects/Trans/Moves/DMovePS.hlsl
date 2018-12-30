#ifndef AVS_SHADER
#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />
#endif
Texture2D<float4> texPrevFrame : register( BIND_PREV_FRAME );
SamplerState ss : register(s1);

float4 main( float2 tc : TEXCOORD0 ) : SV_Target
{
    return texPrevFrame.Sample( ss, tc );
}