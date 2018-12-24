#ifndef AVS_SHADER
static const float4 videoRectangle = float4( -1, -1, 1, 1 );
#endif

struct sOutput
{
    float2 tc : TEXCOORD0;
    float4 pos : SV_Position;
};

sOutput main( uint id : SV_VertexID )
{
    const float2 tc = float2( id & 1, ( id >> 1 ) & 1 );
    sOutput res;
    res.pos.xy = lerp( videoRectangle.xy, videoRectangle.zw, tc );
    res.pos.z = 0.5;
    res.pos.w = 1;
    res.tc = tc;
    return res;
}