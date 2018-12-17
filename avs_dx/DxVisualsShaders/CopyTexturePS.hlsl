Texture2D<float4> texSource : register(t127);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const int3 readPos = int3( int2( screenSpace.xy ), 0 );
    float4 res = texSource.Load( readPos );
    return float4( res.xyz, 1 );
}