RWByteAddressBuffer effectStates : register(u0);
#define STATE_OFFSET 7
#define INIT_STATE 1

[numthreads(1, 1, 1)]
void main()
{
#if INIT_STATE
    effectStates.Store4( STATE_OFFSET, uint4( 0, 1, 0, 0 ) );
#endif
}