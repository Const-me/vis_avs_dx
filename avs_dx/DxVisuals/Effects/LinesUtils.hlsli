cbuffer FrameGlobalData : register(b0)
{
    bool beat : packoffset(c0.x);
    uint currentFrame : packoffset(c0.y);
    uint getTickCount : packoffset(c0.z);
    float deltaTime : packoffset(c0.w);

	// True if g_line_blend_mode is normal or additive i.e. shaders should output alpha channel. False if it e.g. XOR, so shaders instead should output sharp edges and discard() invisible pixels.
    bool lineModeAllowAlpha : packoffset(c1.x);
}

static const float alphaCutoffThreshold = 0.666;

inline float4 alphaCutoffBlend( float3 rgb, float alpha )
{
    float4 result;
    if( alpha >= alphaCutoffThreshold )
        result = float4( rgb, 1 );
    else
    {
        result = 0;
        discard;
    }
    return result;
}

inline float4 alphaBlend( float3 rgb, float alpha )
{
    float4 result;
    if( lineModeAllowAlpha )
        result = float4( rgb * alpha, alpha );
    else
        result = alphaCutoffBlend( rgb, alpha );
    return result;
}