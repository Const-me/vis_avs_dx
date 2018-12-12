#define STATE_OFFSET 7
#define IS_BEAT 1
#define INIT_STATE  0

// These variables will become macros
static const bool onbeat = true;
static const float spdBeat = 4;
static const float WarpSpeed = 6;
static const uint durFrames = 15;

RWByteAddressBuffer effectStates : register(u0);

[numthreads( 1, 1, 1 )]
void main()
{
#if INIT_STATE
	uint nc = 0;
	float CurrentSpeed = WarpSpeed;
	float incBeat = 0;
#else
    uint nc = effectStates.Load( STATE_OFFSET );
    float CurrentSpeed = asfloat( effectStates.Load( STATE_OFFSET + 4 ) );
    float incBeat = asfloat( effectStates.Load( STATE_OFFSET + 8 ) );

    if( 0 == nc )
        CurrentSpeed = WarpSpeed;
    else
    {
        CurrentSpeed = max( 0, CurrentSpeed + incBeat );
        nc--;
    }

    if( onbeat && IS_BEAT )
    {
        CurrentSpeed = spdBeat;
        incBeat = ( WarpSpeed - CurrentSpeed ) / (float) durFrames;
        nc = durFrames;
    }
#endif
    effectStates.Store( STATE_OFFSET, nc );
    effectStates.Store2( STATE_OFFSET + 4, asuint( float2( CurrentSpeed, incBeat ) ) );
}