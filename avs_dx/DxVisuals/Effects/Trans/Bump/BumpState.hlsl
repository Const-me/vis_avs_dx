#define INIT_STATE 0
#define IS_BEAT 0

static const float bumpNormal = 0.5f;
static const float bumpOnBeat = 1.0f;
static const bool onBeat = false;
static const uint durationFrames = 16;

RWByteAddressBuffer effectStates : register(u0);
#define STATE_OFFSET 0

[numthreads(1, 1, 1)]
void main()
{
    if( INIT_STATE )
    {
        effectStates.Store( STATE_OFFSET, 0 );
        effectStates.Store( STATE_OFFSET + 4, asuint( bumpNormal ) );
    }
    else if( onBeat )
    {
        if( IS_BEAT )
        {
            effectStates.Store( STATE_OFFSET, durationFrames );
            effectStates.Store( STATE_OFFSET + 4, asuint( bumpOnBeat ) );
            effectStates.Store( STATE_OFFSET + 20, 1 ); // islbeat
        }
		else
        {
            uint nFrame = effectStates.Load( STATE_OFFSET );
            if( nFrame > 0 )
            {
                nFrame--;
                effectStates.Store( STATE_OFFSET, nFrame );
                float val = lerp( bumpNormal, bumpOnBeat, nFrame * ( 1.0 / durationFrames ) );
                effectStates.Store( STATE_OFFSET + 4, asuint( val ) );
                effectStates.Store( STATE_OFFSET + 20, ( nFrame > 0 ) ? 1 : 0 ); // islbeat
            }
        }
    }
}