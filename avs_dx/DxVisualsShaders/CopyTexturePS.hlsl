Texture2D<float4> texSource : register(t3);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const int3 readPos = int3( int2( screenSpace.xy ), 0 );
    return texSource.Load( readPos );
}