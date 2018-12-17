float4 main( float2 tc : TEXCOORD0, float4 color : COLOR0 ) : SV_Target
{
	// Debug code below - just render quads
    // return color;

    const float len2 = dot( tc, tc );
	// fsolve(exp(x)=1/256,x);
    return color * exp( -5.545177444 * len2 );
}