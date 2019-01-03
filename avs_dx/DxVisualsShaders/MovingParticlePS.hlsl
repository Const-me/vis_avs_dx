float4 main( nointerpolation float2 screenCenter : TEXCOORD0, nointerpolation float4 colorAndSize : COLOR0, float4 screenSpace : SV_Position ) : SV_Target
{
    const float2 dir = ( screenCenter - screenSpace.xy );
    const float len2 = dot( dir, dir ) * colorAndSize.w;
    const float mul = smoothstep( 0.77, 0.73, len2 );
    return float4( colorAndSize.rgb * mul, mul );
}