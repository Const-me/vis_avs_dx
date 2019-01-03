float4 main( nointerpolation float2 screenCenter : TEXCOORD0, nointerpolation float4 colorAndSize : COLOR0, float4 screenSpace : SV_Position ) : SV_Target
{
    // return float4( 1, 1, 1, 1 );
    float2 dir = ( screenCenter - screenSpace.xy );
    const float len2 = dot( dir, dir ) * colorAndSize.w;
	// fsolve(exp(x)=1/64,x);
    float mul = exp( -4.158883083 * len2 );
    return float4( colorAndSize.rgb * mul, mul );
}