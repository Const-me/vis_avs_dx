#ifndef AVS_SHADER
#define BIND_LAST_OUTPUT t7
// <macro-resource name="bindLastOutput" macro="BIND_LAST_OUTPUT" />
#endif
Texture2D<float4> texSource : register(t3);
Texture2D<float4> texLastFrameOutput : register(BIND_LAST_OUTPUT);

inline float3 readSource( int x, int y )
{
    return texSource.Load( int3( x, y, 0 ) ).rgb;
}

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const int2 px = (int2) screenSpace.xy;
    float3 res = readSource( px.x, px.y - 1 );
    res += readSource( px.x - 1, px.y );
    res += readSource( px.x + 1, px.y );
    res += readSource( px.x, px.y + 1 );
    res *= 0.5;
    res -= texLastFrameOutput.Load( int3( px, 0 ) ).rgb;
	return float4( saturate( res ), 1 );
}