#ifndef AVS_SHADER
static const float3 color = float3( 0, 0, 0 );
#endif
float4 main() : SV_Target
{
    return float4( color, 1 );
}