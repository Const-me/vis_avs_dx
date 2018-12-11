#ifndef AVS_SHADER
#define SIZE_X    0.01
#define SIZE_Y    SIZE_X * 16 / 9
#endif

struct sIn
{
    float4 pos : SV_Position;
    float4 color : COLOR0;
};

struct sOut
{
    float4 pos : SV_Position;
    float2 tc : TEXCOORD0;
    float4 color : COLOR0;
};

[maxvertexcount( 4 )]
void main( point sIn input[1], inout TriangleStream<sOut> output )
{
    const float4 color = input[ 0 ].color;
    const float4 pos = input[ 0 ].pos;

    [unroll]
	for( int i = 0; i < 4; i++ )
    {
        const float x = ( i & 1 ) * 2 - 1;
        const float y = ( i & 2 ) - 1;
        sOut r;
        r.pos = pos;
        r.pos.xy += float2( x * SIZE_X, y * SIZE_Y );
        r.tc = float2( x, y );
        r.color = color;
        output.Append( r );
    }
}