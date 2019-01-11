cbuffer FrameGlobalData : register(b0)
{
    bool beat : packoffset(c0.x);
    uint currentFrame : packoffset(c0.y);
    uint getTickCount : packoffset(c0.z);
    float deltaTime : packoffset(c0.w);

	// True if g_line_blend_mode is normal or additive i.e. shaders should output alpha channel. False if it e.g. XOR, so shaders instead should output sharp edges and discard() invisible pixels.
    bool lineModeAllowAlpha : packoffset(c1.x);
}

inline float4 alphaBlend( float3 rgb, float alpha )
{
    float4 result;
    if( lineModeAllowAlpha )
        result = float4( rgb * alpha, alpha );
    else
    {
        if( alpha >= 0.5 )
            result = float4( rgb, 1 );
        else
        {
            result = 0;
            discard;
        }
    }
    return result;
}

float4 main( nointerpolation float2 screenCenter : TEXCOORD0, nointerpolation float4 colorAndSize : COLOR0, float4 screenSpace : SV_Position ) : SV_Target
{
    const float2 dir = ( screenCenter - screenSpace.xy );
    const float len2 = dot( dir, dir ) * colorAndSize.w;
    const float alpha = smoothstep( 0.77, 0.73, len2 );

    return alphaBlend( colorAndSize.rgb, alpha );
}