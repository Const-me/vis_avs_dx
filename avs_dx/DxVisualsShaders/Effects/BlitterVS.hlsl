cbuffer FrameGlobalData : register(b3)
{
    float zoom;
}

struct SimpleVertex
{
    float2 tc : TEXCOORD0;
    float4 pos : SV_Position;
};

SimpleVertex main( uint id : SV_VertexID )
{
    float2 xy = float2( id & 1, ( id >> 1 ) );
    SimpleVertex res;
    res.tc = xy;
    xy = xy * float2( 2, -2 ) - 1;
    xy *= zoom;
    res.pos.xy = xy;
    res.pos.zw = float2( 0.5, 1 );
    return res;
}