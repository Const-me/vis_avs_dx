#include "FrameGlobalData.fx"
#include "Star.fx"
#ifndef AVS_SHADER
#define BIND_STARS_POSITIONS u4
// <macro-resource name="bindStarsPosition" macro="BIND_STARS_POSITIONS" type="uav" />
#define EFFECT_STATE 0
#endif
RWBuffer<StarFormat> stars : register( BIND_STARS_POSITIONS );

[numthreads( 256, 1, 1 )]
void main()
{
	
}