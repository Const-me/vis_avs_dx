float4 main(uint id : SV_VertexID) : SV_Position
{
	// https://web.archive.org/web/20140719063725/http://www.altdev.co/2011/08/08/interesting-vertex-shader-trick/
    const float2 tc = float2((id << 1) & 2, id & 2);
    return float4(tc * float2(2, -2) + float2(-1, 1), 0.5, 1);
}