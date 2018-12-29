#ifndef AVS_SHADER
static const float3 color = float3( 1, 1, 1 );
static const uint invert = 0;
#endif
Texture2D<float4> texPrevFrame : register(t4);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const float4 src = texPrevFrame.Load( int3( int2( screenSpace.xy ), 0 ) );
    float val = max( max( src.x, src.y ), src.z );
    if( invert )
        val = 1.0 - val;
    return float4( color * val, 1.0 );
}