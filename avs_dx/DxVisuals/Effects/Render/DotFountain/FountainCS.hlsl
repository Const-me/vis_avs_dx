#include "VisCommon.fx"
#include "Fountain.fx"
#ifndef AVS_SHADER
#define BIND_PARTICLES u4
// <macro-resource name="bindParticles" macro="BIND_PARTICLES" type="uav" />
#define COLOR_VALUES float3( 1, 1, 1 ), float3( 1, 1, 1 ), float3( 1, 1, 1 ), float3( 1, 1, 1 ), float3( 1, 1, 1 )
// <macro-array name="colors" type="float3" size="5" values="COLOR_VALUES" />

static const float gravity = 1;
static const float airResistance = 0.0333f;
static const float2 speedInitial = float2( 0.25f, 1.0f );
static const float speedMul = 2;
static const float speedMulBeat = 4;
static const float2 posInitial = float2( 0.04f, -0.33f );

#endif
RWStructuredBuffer<FountainPoint> particles : register(BIND_PARTICLES);

inline float3 particleColor( float t )
{
    const float3 values[ 5 ] = { COLOR_VALUES };
    t = saturate( t );
    t *= 4.0;
    uint i1 = (uint) floor( t );
    if( 4 == i1 )
        i1 = 3;
    t = t - (float) i1;
    return lerp( values[ i1 ], values[ i1 + 2 ], t );
}

inline float3 randomSpeedAdjust( float3 spd, uint dir )
{
    uint rng_state = srand( dir ^ currentFrame );
    float3 adj = float3( randf( rng_state ), randf( rng_state ), randf( rng_state ) );
    return spd + ( adj - 0.5 ) * 0.01;
}

inline FountainPoint createNew( FountainPoint fp, uint dir )
{
    const float sampleMul = 2.0f / 576;
    float t = sampleVisData( float2( (float) dir * sampleMul, 0.25 ), false );

#if IS_BEAT
	float spMul = lerp( 1, speedMulBeat, t );
#else
    float spMul = lerp( 1, speedMul, t );
#endif

    fp.color = particleColor( t );
    fp.position = float3( posInitial.x * fp.xyDir, posInitial.y );
    float2 speed2 = speedInitial * spMul;
    fp.speed = float3( speed2.x * fp.xyDir, speed2.y );
    fp.speed = randomSpeedAdjust( fp.speed, dir );
    return fp;
}

inline FountainPoint transform( FountainPoint fp )
{
    FountainPoint p = fp;
    const float3 forceGravity = float3( 0, 0, -gravity );
    float3 speedDir = normalize( fp.speed );
    const float3 forceAir = -speedDir * ( dot( fp.speed, fp.speed ) * airResistance );
    float3 newSpeed = p.speed + deltaTime * ( forceGravity + forceAir );
    p.position += ( p.speed + newSpeed ) * ( 0.5 * deltaTime );
    p.speed = newSpeed;
    return p;
}

[numthreads( NUM_ROT_DIV, 1, 1 )]
void main( uint3 groupIndex : SV_GroupID, uint3 localThread : SV_GroupThreadID, uint3 globalThread : SV_DispatchThreadID )
{
    const uint gen = groupIndex.x;
    const uint time = currentFrame % NUM_ROT_HEIGHT; // currentFrame is from the globally accessible cbuffer in FrameGlobalData.fx
    const uint id = globalThread.x;

    FountainPoint fpOld = particles[ id ];
    if( gen == time )
    {
		// Launching a new generation of particles
        particles[ id ] = createNew( fpOld, localThread.x );
    }
    else
    {
		// Updating this generation
        particles[ id ] = transform( fpOld );
    }
}