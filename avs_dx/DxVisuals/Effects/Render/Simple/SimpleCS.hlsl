#include "FrameGlobalData.fx"
#ifndef AVS_SHADER
#define BIND_DOTS_POSITIONS u4
// <macro-resource name="bindDotsPosition" macro="BIND_DOTS_POSITIONS" type="uav" />
#define EFFECT_STATE 0
#endif
RWBuffer<float2> dots : register( BIND_DOTS_POSITIONS );

[numthreads( 256, 1, 1 )]
void main()
{
	
}