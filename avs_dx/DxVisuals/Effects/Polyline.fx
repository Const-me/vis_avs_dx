struct sVertex1
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

struct sVertex2
{
    float4 color : COLOR0;
	// 0 = on the original line, -1 = on the left edge, +1 = on the right edge, or maybe vice versa :-)
    // float texCoord : TEXCOORD0;
	// Coordinates of the original line segment, in screen space; xy is first point, zw second
    nointerpolation float4 segmentPoints : TEXCOORD1;
    float4 position : SV_Position;
};

inline float lengthSquared( float2 v )
{
    return dot( v, v );
}