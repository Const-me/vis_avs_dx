#ifndef AVS_SHADER

static const uint2 size = uint2( 1280, 720 );

#define KERNEL_VALUES 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
// <macro-array name="kernel" type="float" size="49" values="KERNEL_VALUES" />

#define BIND_SOURCE t3
// <macro-resource name="bindInput" macro="BIND_SOURCE" />

#define BIND_DEST u3
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />

static const bool wrap = true;

// Largest value that doesn't produce compiler warning X4714: sum of temp registers exceeds the recommended total 16384. Performance may be reduced.
static const uint threadsPerGroup = 16;
#endif

Texture2D<float4> texSource : register(BIND_SOURCE);
RWTexture2D<float4> texDest : register(BIND_DEST);

groupshared float3 s_cache[ threadsPerGroup ][ threadsPerGroup ];

inline int wrapValue( int v, int ax )
{
    if( v < 0 )
        v += ax;
    else if( v >= ax )
        v -= ax;
    return v;
}

[numthreads( threadsPerGroup, threadsPerGroup, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 localThread : SV_GroupThreadID )
{
    const int2 writesPerGroup = int2( threadsPerGroup - 6, threadsPerGroup - 6 ); //< constexpr
    const int2 tid = localThread.xy;

	// Load the source data
	{
        int3 loadPos;

        loadPos.xy = writesPerGroup * groupIndex.xy + tid - int2( 3, 3 );
        if( wrap )
        {
            loadPos.x = wrapValue( loadPos.x, size.x );
            loadPos.y = wrapValue( loadPos.y, size.y );
        }
        else
            loadPos.xy = clamp( loadPos.xy, int2( 0, 0 ), size - 1 );

        loadPos.z = 0;

        s_cache[ tid.y ][ tid.x ] = texSource.Load( loadPos ).rgb;
    }

	// Wait for the whole group to finish running the above code
    GroupMemoryBarrierWithGroupSync();

	// Now the cache is ready for the whole group. Compute the convolution.
    const uint2 destIndex = groupIndex.xy * writesPerGroup + (uint2) tid - uint2( 3, 3 );

	// Skip non-central parts of the group, we don't have enough cached data to compute them.
	// Also skip pixels on the right and bottom edges that were added for rounding.
    if( any( tid < 3 || (uint2) tid >= threadsPerGroup - 3 || destIndex >= size ) )
        return;

    const float kernelValues[ 49 ] = { KERNEL_VALUES };

    float3 res = 0;
	[unroll]
    for( int y = -3; y <= 3; y++ )
    {
		[unroll]
        for( int x = -3; x <= 3; x++ )
        {
            const uint ind = ( y + 3 ) * 7 + x + 3;
            res += kernelValues[ ind ] * s_cache[ tid.y + y ][ tid.x + x ];
        }
    }

    texDest[ destIndex ] = float4( res, 1 );
}