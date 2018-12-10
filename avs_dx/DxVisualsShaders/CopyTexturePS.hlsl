Texture2D<float4> texSource : register(t127);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    return texSource.Load( int3( int2( screenSpace.xy ), 0 ) );
}