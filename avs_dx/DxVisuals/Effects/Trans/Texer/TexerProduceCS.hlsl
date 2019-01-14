#ifndef AVS_SHADER
static const uint2 sizeImage = uint2( 1280, 720 );

#define BIND_DEST u0
// <macro-resource name="bindOutput" macro="BIND_DEST" type="uav" />

#endif

Texture2D<uint4> source : register(t3);
AppendStructuredBuffer<float2> destBuffer : register(BIND_DEST);

#define size   ( ( sizeImage + 7 ) / 8 )

inline uint shouldSkip( uint4 cell, uint2 neighbour )
{
    const uint4 n = source[ neighbour ];
    int2 distVec = (int2) cell.xy - (int2) n.xy;
    uint distSquare = (uint) ( distVec.x * distVec.x ) + (uint) ( distVec.y * distVec.y );
    
    if( distSquare > 16 )
        return 0;
	else
	    return cell.z < n.z;
}

[numthreads( size.x, 1, 1 )]
void main( uint3 globalThread : SV_DispatchThreadID )
{
	// Possible to optimize with local memory, however the size is already small here, 240x135 for FullHD, the simpler method will prolly do fine.

    const uint2 tid = globalThread.xy;
    const uint4 cell = source[ tid ];

    uint skip = 0;

    if( globalThread.x > 0 )
        skip |= shouldSkip( cell, tid - uint2( 1, 0 ) );
    if( globalThread.y > 0 )
        skip |= shouldSkip( cell, tid - uint2( 0, 1 ) );

    if( globalThread.x + 1 <= size.x )
        skip |= shouldSkip( cell, tid + uint2( 1, 0 ) );
    if( globalThread.y + 1 <= size.y )
        skip |= shouldSkip( cell, tid + uint2( 0, 1 ) );

    if( 0 == skip )
    {
        const float2 px = float2( cell.xy );
        const float2 mul = float2( 2, -2 ) / sizeImage;
        const float2 clipSpace = px * mul - 1;
        destBuffer.Append( clipSpace );
    }
}