#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif
#define halfSize ( 0.5 * AVS_RENDER_SIZE )

struct sIn
{
    float2 pos : SV_Position;
    float4 colorAndSize : COLOR0;
};

struct sOut
{
    float4 pos : SV_Position;
    float4 colorAndSize : COLOR0;
};

sOut main( sIn p )
{
    float2 v = p.pos;
    v *= ( 2.0 / AVS_RENDER_SIZE );	// Now in [0..2]
    v -= 1;
    v *= float2( 1, -1 );

    sOut res;
    res.pos = float4( v, 0.5, 1 );
    res.colorAndSize = p.colorAndSize;
    return res;
}