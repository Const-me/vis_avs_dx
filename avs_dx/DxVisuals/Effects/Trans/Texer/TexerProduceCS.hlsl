#ifndef AVS_SHADER
static const uint2 sizeImage = uint2( 1280, 720 );
#define BIND_DEST u0
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />
#endif

Texture2D<uint> source : register(t3);
AppendStructuredBuffer<float2> destBuffer : register(BIND_DEST);

[numthreads( 8, 8, 1 )]
void main( uint3 globalThread : SV_DispatchThreadID )
{
    const uint2 src = globalThread.xy;
    if( any( src >= sizeImage ) )
        return;

    const uint isMax = source[ src ];
    if( isMax )
    {
        const float2 px = float2( src );
        const float2 mul = float2( 2, -2 ) / sizeImage;
        const float2 clipSpace = px * mul + float2( -1, 1 );
        destBuffer.Append( clipSpace );
    }
}