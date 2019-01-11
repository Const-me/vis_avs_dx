struct sVertex1
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

struct sVertex2
{
    float4 color : COLOR0;
	// Coordinates of the line segment, in screen space; xy is first point, zw second
    nointerpolation float4 segmentPoints : TEXCOORD1;
    float4 position : SV_Position;
};

inline float lengthSquared( float2 v )
{
    return dot( v, v );
}