#ifndef AVS_SHADER
static const uint2 sizeImage = uint2( 1280, 720 );
#define BIND_DEST u0
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />
#endif

Texture2D<float4> texSource : register(t3);
RWTexture2D<uint4> texDest : register(BIND_DEST);

groupshared float s_cache[ 8 ][ 8 ];

[numthreads( 8, 8, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 tid : SV_GroupThreadID, uint3 globalThread : SV_DispatchThreadID )
{
    const bool withinBounds = all( globalThread.xy < sizeImage );
    float val = 0;
    if( withinBounds )
    {
        const float3 src = texSource[ globalThread.xy ].rgb;
        val = src.r + src.g + src.b;
        s_cache[ tid.y ][ tid.x ] = val;
    }

    if( all( tid == 0 ) )
        texDest[ groupIndex.xy ] = 0;

    GroupMemoryBarrierWithGroupSync();

    if( !withinBounds )
        return;

    bool isMax = true;

    isMax = isMax && ( val >= s_cache[ tid.y ][ tid.x ^ 4 ] );
    isMax = isMax && ( val >= s_cache[ tid.y ^ 4 ][ tid.x ] );

    isMax = isMax && ( val >= s_cache[ tid.y ][ tid.x ^ 2 ] );
    isMax = isMax && ( val >= s_cache[ tid.y ^ 2 ][ tid.x ] );

    isMax = isMax && ( val >= s_cache[ tid.y ][ tid.x ^ 1 ] );
    isMax = isMax && ( val >= s_cache[ tid.y ^ 1 ][ tid.x ] );

    if( isMax && val > 0 )
    {
        const float2 px = float2( globalThread.xy );
        const float2 mul = float2( 2, -2 ) / sizeImage;
        const float2 clipSpace = px * mul - 1;
        uint4 res;
        res.xy = globalThread.xy;
        res.z = (uint) ( val * 1024.0 );
        res.w = 0;
        texDest[ groupIndex.xy ] = res;
    }
}