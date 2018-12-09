#ifndef AVS_SHADER
#define SHADER_CODE
#endif

RWByteAddressBuffer effectStates : register(u0);

[numthreads( 1, 1, 1 )]
void main()
{
    uint currentFrame = effectStates.Load( 0 );
    currentFrame++;
    effectStates.Store( 0, currentFrame );

	SHADER_CODE
}