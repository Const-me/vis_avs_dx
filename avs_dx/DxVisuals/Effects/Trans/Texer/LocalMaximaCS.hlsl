#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#define BIND_DEST u0
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />
#endif

Texture2D<float4> texSource : register(t3);

#define sizeImage uint2( AVS_RENDER_SIZE )

struct sSprite
{
    float2 position;
    float3 color;
};
AppendStructuredBuffer<sSprite> destBuffer : register(BIND_DEST);

groupshared float s_cache[ 16 ][ 16 ];
groupshared uint s_anyPixel = 0;

inline void updateGe( inout bool isMax, float val, uint3 tid, int dx, int dy )
{
    const bool res = val >= s_cache[ tid.y + dy ][ tid.x + dx ];
    isMax = isMax && res;
}
inline void updateG( inout bool isMax, float val, uint3 tid, int dx, int dy )
{
    const bool res = val > s_cache[ tid.y + dy ][ tid.x + dx ];
    isMax = isMax && res;
}

[numthreads( 16, 16, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 tid : SV_GroupThreadID )
{
    const uint2 blockWritePos = groupIndex.xy * uint2( 14, 14 );
    const uint2 locPixel = blockWritePos + tid.xy - 1;

    float val;
    float3 srcColor = 0;
    if( all( locPixel < sizeImage ) )
    {
        srcColor = texSource[ locPixel ].rgb;
        val = srcColor.r + srcColor.g + srcColor.b;
    }
    else
        val = 0;

    s_cache[ tid.y ][ tid.x ] = val;

    if( val > 0.01 )
        InterlockedOr( s_anyPixel, 1 );

    GroupMemoryBarrierWithGroupSync();

    if( 0 == s_anyPixel )
        return;

    if( any( locPixel >= sizeImage || tid.xy - 1 >= 14 ) )
        return;

    bool isMax = true;

    updateG( isMax, val, tid, -1, -1 );
    updateG( isMax, val, tid, 0, -1 );
    updateG( isMax, val, tid, +1, -1 );

    updateG( isMax, val, tid, -1, 0 );
    updateGe( isMax, val, tid, +1, 0 );

    updateGe( isMax, val, tid, -1, +1 );
    updateGe( isMax, val, tid, 0, +1 );
    updateGe( isMax, val, tid, +1, +1 );

    if( !isMax )
        return;

    sSprite newSprite;

    newSprite.color = srcColor;

    const float2 px = float2( locPixel );
    const float2 mul = float2( 2, -2 ) / sizeImage;
    newSprite.position = px * mul + float2( -1, 1 );

    destBuffer.Append( newSprite );
}