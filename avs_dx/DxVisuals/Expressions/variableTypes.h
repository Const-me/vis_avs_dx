#pragma once

// HLSL template engine uses these types in shader structures to bridge from HLSL to C++.
using float2 = Vector2;
using float3 = Vector3;
using float4 = Vector4;
using uint2 = CSize;
using int2 = CPoint;
using uint = uint32_t;

// Convert Windows COLORREF into float3
inline float3 float3FromColor( int rgb )
{
	float3 res;
	res.x = (float)( ( rgb >> 16 ) & 0xFF );
	res.y = (float)( ( rgb >> 8 ) & 0xFF );
	res.z = (float)( rgb & 0xFF );
	return res * ( 1.0f / 255.0f );
}
// Convert Windows COLORREF into float4 with specified alpha
inline float4 float4FromColor( int rgb, float alpha )
{
	const float3 v = float3FromColor( rgb );
	return float4( v.x, v.y, v.z, alpha );
}

inline float2 floatSize( const CSize& size )
{
	return float2{ (float)size.cx, (float)size.cy };
}

using uint4 = std::array<uint32_t, 4>;