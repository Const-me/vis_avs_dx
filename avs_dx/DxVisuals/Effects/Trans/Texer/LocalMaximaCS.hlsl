#ifndef AVS_SHADER
static const uint2 sizeImage = uint2( 1280, 720 );
#define BIND_DEST u0
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />
#endif

Texture2D<float4> texSource : register(t3);
RWTexture2D<uint> texDest : register(BIND_DEST);

groupshared float s_cache[ 16 ][ 16 ];

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
    if( all( locPixel < sizeImage ) )
    {
        const float3 src = texSource[ locPixel ].rgb;
        val = src.r + src.g + src.b;
    }
    else
        val = 0;

    s_cache[ tid.y ][ tid.x ] = val;

    GroupMemoryBarrierWithGroupSync();

    if( any( locPixel >= sizeImage || tid.xy - 1 >= 14 ) )
        return;

    if( val < 0.01 )
    {
        texDest[ locPixel ] = 0;
        return;
    }

    bool isMax = true;

    updateG( isMax, val, tid, -1, -1 );
    updateG( isMax, val, tid, 0, -1 );
    updateG( isMax, val, tid, +1, -1 );

    updateG( isMax, val, tid, -1, 0 );
    updateGe( isMax, val, tid, +1, 0 );

    updateGe( isMax, val, tid, -1, +1 );
    updateGe( isMax, val, tid, 0, +1 );
    updateGe( isMax, val, tid, +1, +1 );

    texDest[ locPixel ] = isMax;
}