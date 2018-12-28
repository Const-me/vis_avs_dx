#ifndef AVS_SHADER
static const uint curtrans = 1;
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif

Texture2D<float4> texSource : register(t125);
Texture2D<float4> texDest : register(t126);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    return float4( 0, 0, 0, 0 );

}