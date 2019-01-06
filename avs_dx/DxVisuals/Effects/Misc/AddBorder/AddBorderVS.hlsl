#ifndef AVS_SHADER
#define BIND_CB b3
// <macro-resource name="bindConstBuffer" macro="BIND_CB" type="cbuffer" />
#endif

cbuffer AddBorderCb : register(BIND_CB)
{
    float3 color : packoffset(c0.x);
    float amount : packoffset(c0.w);
}

float4 main( uint id : SV_VertexID ) : SV_Position
{
    const float2 verts[ 4 ] =
    {
        float2( -1, -1 ),
        float2( +1, -1 ),
        float2( +1, +1 ),
        float2( -1, +1 ),
    };
    float2 vec = verts[ id >> 1 ];
	if( id & 1 )
        vec *= ( 1.0 - amount );
    return float4( vec, 0.5, 1 );
}