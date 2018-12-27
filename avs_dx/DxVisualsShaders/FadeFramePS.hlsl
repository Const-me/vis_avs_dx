Texture2D<float4> texSource : register(t126);

float4 main( float4 screenSpace : SV_Position ) : SV_Target
{
    const int3 readPos = int3( int2( screenSpace.xy ), 0 );
    float4 c = texSource.Load( readPos );
    const float fadeFactor = 1.0 / 255;
    c = c * ( 1.0 + fadeFactor ) - fadeFactor;
    return saturate( c );
}