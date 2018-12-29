#define STATE_OFFSET 7
static const uint beatdur = 4;
static const uint2 sizeNormal = uint2( 1, 1 );
static const uint2 sizeOnBeat = uint2( 1, 1 );

RWByteAddressBuffer effectStates : register(u0);

[numthreads( 1, 1, 1 )]
void main()
{
    float2 currentFloat;
#if INIT_STATE
	currentFloat = float2( sizeNormal );
#else
    if( sizeNormal.x != sizeOnBeat.x || sizeNormal.y != sizeOnBeat.y )
    {
#if IS_BEAT
		currentFloat = float2( sizeOnBeat );
#else
        currentFloat = asfloat( effectStates.Load2( STATE_OFFSET + 8 ) );
        const float sc1 = ( beatdur + 512.0 - 64.0 ) / 512.0;
        currentFloat = currentFloat * sc1 + float2( sizeNormal ) * ( 1.0 - sc1 );
#endif
    }
	else
        currentFloat = float2( sizeNormal );

#endif
    uint2 currentUint = (uint2) round( currentFloat );
    effectStates.Store2( STATE_OFFSET, currentUint );
    effectStates.Store2( STATE_OFFSET + 8, asuint( currentFloat ) );
}