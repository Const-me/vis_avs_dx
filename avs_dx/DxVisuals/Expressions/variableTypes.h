#pragma once

// HLSL template engine uses these types in shader structures to bridge from HLSL to C++.
using float2 = Vector2;
using float3 = Vector3;
using float4 = Vector4;
using uint2 = CSize;
using int2 = CPoint;
using uint = uint32_t;

// Convert Window COLORREF into float3
inline float3 float3FromColor( int rgb )
{
	float3 res;
	res.x = (float)( ( rgb >> 16 ) & 0xFF );
	res.y = (float)( ( rgb >> 8 ) & 0xFF );
	res.z = (float)( rgb & 0xFF );
	return res * ( 1.0f / 255.0f );
}