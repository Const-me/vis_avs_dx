#ifndef AVS_SHADER
static const uint WARP_SIZE = 32;
static const uint2 size = uint2( 1280, 720 );
static const uint warpsPerGroup = 4;

#define kernelRadius 3
#define KERNEL_VALUES 0.402619946894247, 0.244201342003233, 0.0544886845496429

#define BIND_SOURCE t4
// <macro-resource name="bindSource" macro="BIND_SOURCE" />

#define BIND_DEST u1
// <macro-resource name="bindDest" macro="BIND_DEST" type="uav" />

#endif

Texture2D<float4> texSource : register(BIND_SOURCE);
RWTexture2D<float4> texDest : register(BIND_DEST);

// The code below implements arbitrary-size Gaussian blur with slightly more than 1 texture load per pass per pixel (2 passes are required for 2D images).
// This is better than what I saw on the Internets, the state of the art appears to be abusing bilinear filter units, or using worse-quality approximations like Kawase's.

groupshared float3 s_cache[ WARP_SIZE * warpsPerGroup ];

[numthreads( WARP_SIZE * warpsPerGroup, 1, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 localThread : SV_GroupThreadID, uint3 globalThread : SV_DispatchThreadID )
{
    const uint groupSize = WARP_SIZE * warpsPerGroup; //< constexpr
    const uint writesPerGroup = groupSize + 2 - kernelRadius - kernelRadius; //< constexpr
    const uint tid = localThread.x;

	// Load the source data
	{
        int3 loadPos;
        loadPos.x = writesPerGroup * groupIndex.x + tid;
        if( loadPos.x >= (int) size.x )
            loadPos.x = size.x - 1;
        loadPos.y = groupIndex.y;
        loadPos.z = 0;
        s_cache[ tid ] = texSource.Load( loadPos ).rgb;
    }

	// Wait for the whole group to finish running the above code
    GroupMemoryBarrierWithGroupSync();

	// Now the cache is ready for the whole group. It's local memory i.e. much faster to access than textures. Compute the convolution.
    const float kernelValues[ kernelRadius ] = { KERNEL_VALUES };

    if( 0 == groupIndex.x )
    {
        if( tid > ( groupSize - kernelRadius ) )
            return;

        float3 res = 0;
		[unroll]
        for( uint i = kernelRadius - 1; i > 0; i-- )
        {
            const float k = kernelValues[ i ];
            float3 v = s_cache[ tid + i ];
            const uint prevInd = ( tid >= i ) ? tid - i : 0;
            v += s_cache[ prevInd ];
            res += v * k;
        }
		// Central pixel
        res += s_cache[ tid ] * kernelValues[ 0 ];

		// Write the result
        texDest[ uint2( groupIndex.y, tid ) ] = float4( res, 1 );
    }
    else
    {
        const uint dest = writesPerGroup * groupIndex.x + tid;
        if( tid < ( kernelRadius - 1 ) || tid > ( groupSize - kernelRadius ) || dest >= size.x )
            return;

        float3 res = 0;
		// Start from outside for improved precision: kernel has much smaler values on the outside, and for performance reasons we're using single-precision floats here.
		[unroll]
        for( uint i = kernelRadius - 1; i > 0; i-- )
        {
            const float k = kernelValues[ i ];
            res += ( s_cache[ tid - i ] + s_cache[ tid + i ] ) * k;
        }
		// Central pixel
        res += s_cache[ tid ] * kernelValues[ 0 ];

		// Write the result
        texDest[ uint2( groupIndex.y, dest ) ] = float4( res, 1 );
    }
}