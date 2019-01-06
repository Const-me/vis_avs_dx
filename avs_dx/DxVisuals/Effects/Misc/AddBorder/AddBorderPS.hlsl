#ifndef AVS_SHADER
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
#endif

cbuffer AddBorderCb : register(BIND_CB)
{
    float3 color : packoffset(c0.x);
    float amount : packoffset(c0.w);
}

float4 main() : SV_Target
{
    return float4( color, 1 );
}