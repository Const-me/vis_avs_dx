#ifndef AVS_SHADER
static const float2 sizeInPixels = float2( 8, 8 );
#define AVS_RENDER_SIZE float2( 1280, 720 )
#endif

struct sIn
{
    float4 pos : SV_Position;
};
struct sOut
{
    float2 tc : TEXCOORD0;
    float4 pos : SV_Position;
};

[maxvertexcount( 4 )]
void main( point sIn input[ 1 ], inout TriangleStream<sOut> output )
{
    const float4 pos = input[ 0 ].pos;
    const float2 halfSize = sizeInPixels / AVS_RENDER_SIZE;

    [unroll]
    for( int i = 0; i < 4; i++ )
    {
        const float2 xy = float2( ( i & 1 ) * 2.0 - 1.0, 1.0 - ( i & 2 ) );
        sOut r;
        r.pos = pos;
        r.pos.xy += xy * halfSize * pos.ww;
        r.tc = xy * 0.5 + 0.5;
        output.Append( r );
    }
}