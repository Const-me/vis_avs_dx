#ifndef AVS_SHADER
static const float3 color = float3( 1, 1, 1 );
static const float fadelen = 16.0f / 256.0f;
#endif
Texture2D<float4> texPrevFrame : register(t4);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	// https://www.wolframalpha.com/input/?i=plot(Piecewise%5B%7B%7Bx%2B0.1,+x%3C0.4%7D,%7B0.5,0.4%3Cx%3C0.6%7D,%7Bx-0.1,+x%3E0.6%7D%7D%5D,%7Bx,0,1%7D)

    const float3 src = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) ).xyz;
    const float3 clamped = clamp( src, color - fadelen, color + fadelen );
    return float4( src + color - clamped, 1 );
}