float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
	// Debug code below - just render quads
    // return color;

    const float len2 = dot( tc, tc );
    return color * exp( -4 * len2 );
}