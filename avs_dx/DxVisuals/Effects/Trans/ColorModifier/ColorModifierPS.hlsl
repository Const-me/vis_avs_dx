#ifndef AVS_SHADER
#define SHADER_GLOBALS
#define SHADER_CODE

#define BIND_PREV_FRAME t1
// <macro-resource name="bindPrevFrame" macro="BIND_PREV_FRAME" />

#endif
Texture2D<float4> texPrevFrame : register( BIND_PREV_FRAME );

SHADER_GLOBALS

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	const float4 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
	float red = color.x;
	float green = color.y;
	float blue = color.z;
	SHADER_CODE
	return float4( red, green, blue, 1 );
}