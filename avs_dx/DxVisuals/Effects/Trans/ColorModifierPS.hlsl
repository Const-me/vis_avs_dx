#ifndef AVS_SHADER
#define SHADER_CODE
#define BIND_PREVFRAME t1
#endif

Texture2D<float4> texPrevFrame : register( BIND_PREVFRAME );

float4 main( float4 screenSpace: SV_Position ) : SV_Target
{
    const float4 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
    float red = color.x;
    float green = color.y;
    float blue = color.z;
	SHADER_CODE
    return float4( red, green, blue, 1 );
}