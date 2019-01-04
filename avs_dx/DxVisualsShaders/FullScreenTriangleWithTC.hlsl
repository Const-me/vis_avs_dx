struct sVertex
{
    float2 tc: TEXCOORD0;
    float4 pos : SV_Position;
};

sVertex main(uint id : SV_VertexID)
{
	// https://web.archive.org/web/20140719063725/http://www.altdev.co/2011/08/08/interesting-vertex-shader-trick/
    sVertex v;
    v.tc = float2((id << 1) & 2, id & 2);
    v.pos = float4( v.tc * float2( 2, -2 ) + float2( -1, 1 ), 0.5, 1 );
    return v;
}