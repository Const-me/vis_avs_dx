#ifndef AVS_SHADER
#define AVS_RENDER_SIZE float2( 1280, 720 )
static const float particleSizeMultiplier = 2;
#endif

struct sIn
{
    float4 pos : SV_Position;
    float4 colorAndSize : COLOR0;
};

struct sOut
{
    nointerpolation float2 screenCenter : TEXCOORD0;
    nointerpolation float4 colorAndSize : COLOR0;
    float4 pos : SV_Position;
};

inline sOut vertex( float2 csPos, float2 pxOffset, float4 colorAndSize, float2 pxCenter )
{
    sOut r;
    r.screenCenter = pxCenter;
    r.colorAndSize = colorAndSize;
    r.pos = float4( csPos + pxOffset * ( 2.0 / AVS_RENDER_SIZE ), 0.5, 1 );
    return r;
}

[maxvertexcount( 18 )]
void main( point sIn input[ 1 ], inout TriangleStream<sOut> output )
{
    float4 colorAndSize = input[ 0 ].colorAndSize;
    colorAndSize.w *= particleSizeMultiplier;

    const float pixelSize = colorAndSize.w;
	// Invert size here for faster PS: this shader only runs once, PS for every pixel, can be thousands of them.
    colorAndSize.w = 1.0 / ( colorAndSize.w * colorAndSize.w );

    float2 csCenter = input[ 0 ].pos.xy;
    float2 pxCenter = ( ( csCenter * float2( 0.5, -0.5 ) ) + 0.5 ) * AVS_RENDER_SIZE;
    sOut center = vertex( csCenter, float2( 0, 0 ), colorAndSize, pxCenter );

	[unroll]
    for( uint i = 0; i < 6; i++ )
    {
        float2 dir1, dir2;
        sincos( radians( i * 60 ), dir1.y, dir1.x );
        sincos( radians( ( i + 1 ) * 60 ), dir2.y, dir2.x );

        output.Append( vertex( csCenter, dir1 * pixelSize, colorAndSize, pxCenter ) );
        output.Append( center );
        output.Append( vertex( csCenter, dir2 * pixelSize, colorAndSize, pxCenter ) );
        output.RestartStrip();
    }
}