#ifndef AVS_SHADER
#define BIND_TABLE t6
// <macro-resource name="bindTable" macro="BIND_TABLE" />
#endif
Texture2D<float4> texPrevFrame : register(t3);
Texture1D<float4> table : register(BIND_TABLE);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
	const float4 color = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
    uint3 pos = uint3( color.rgb * 1023.0 );

    float4 rgb;
    rgb.r = table.Load( int2( pos.r, 0 ) ).r;
    rgb.g = table.Load( int2( pos.g, 0 ) ).g;
    rgb.b = table.Load( int2( pos.b, 0 ) ).b;
    rgb.a = 1;
    return rgb;
}