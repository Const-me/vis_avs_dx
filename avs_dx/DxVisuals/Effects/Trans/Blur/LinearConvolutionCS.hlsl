#ifndef AVS_SHADER
static const uint WARP_SIZE = 32;
static const uint2 size = uint2( 1280, 720 );

#define kernelRadius 3
#define KERNEL_VALUES 0.402619946894247, 0.244201342003233, 0.0544886845496429

#define BIND_SOURCE t4
// <macro-resource name="bindSource" macro="BIND_SOURCE" />

#define BIND_DEST u1
// <macro-resource name="bindDest" macro="BIND_DEST" type="uav" />

#endif

Texture2D<float4> texSource : register(BIND_SOURCE);
RWTexture2D<float4> texDest : register(BIND_DEST);

// The code below implements arbitrary-size Gaussian blur with just 2 loads per pass per pixel (2 passes are required for 2D images).
// This is much faster than what I saw on the Internets, the state of the art appears to be abusing bilinear filter units, or using worse-quality approximations like Kawase's.

groupshared float3 s_cache[ WARP_SIZE * 4 ]; // 1536 bytes of cache, not that much.

[numthreads( WARP_SIZE * 2, 1, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 localThread : SV_GroupThreadID, uint3 globalThread : SV_DispatchThreadID )
{
    const uint2 pixel = globalThread.xy;

	// Load the source data
	{
        const uint t = localThread.x;
        int3 loadPos;
        loadPos.y = pixel.y;
        loadPos.z = 0;

		// Load WARP_SIZE elements before this group, into cache locations 0 .. WARP_SIZE
        if( t < WARP_SIZE )	//< This branch passes or fails for complete warp i.e. is free.
        {
            loadPos.x = ( groupIndex.x > 0 ) ? ( (int) pixel.x - WARP_SIZE ) : 0;
            s_cache[ t ] = texSource.Load( loadPos ).rgb;
        }

		// Load WARP_SIZE*2 elements for this group, into cache locations WARP_SIZE .. WARP_SIZE*3
		{
            loadPos.x = ( pixel.x < size.x ) ? (int) pixel.x : ( (int) size.x - 1 );
            s_cache[ t + WARP_SIZE ] = texSource.Load( loadPos ).rgb;
        }
        
		// Load WARP_SIZE elements after this group, into cache locations WARP_SIZE*3 .. WARP_SIZE*4
        if( t >= WARP_SIZE )	//< This branch passes or fails for complete warp i.e. is free.
        {
            uint nextPos = pixel.x + WARP_SIZE;
            if( nextPos >= size.x )
                nextPos = size.x - 1;
            loadPos.x = nextPos;
            s_cache[ t + WARP_SIZE * 2 ] = texSource.Load( loadPos ).rgb;
        }
    }

	// Wait for the whole group to finish running the above code
    GroupMemoryBarrierWithGroupSync();

	// Now the cache is ready for the whole group. It's local memory i.e. much faster to access than textures. Compute the convolution.
    const uint cacheIndex = localThread.x + WARP_SIZE;
    const float kernelValues[ kernelRadius ] = { KERNEL_VALUES };

    float3 res = 0;
	// Start from outside for improved precision: kernel has much smaler values on the outside, and for performance reasons we're using single-precision floats here.
	[unroll]
    for( uint i = kernelRadius-1; i > 0; i-- )
    {
        const float k = kernelValues[ i ];
        res += ( s_cache[ cacheIndex - i ] + s_cache[ cacheIndex + i ] ) * k;
    }
	// Central pixel
    res += s_cache[ cacheIndex ] * kernelValues[ 0 ];

	// Write the result. Note the transposition.
    texDest[ uint2( pixel.y, pixel.x ) ] = float4( res, 1 );
}