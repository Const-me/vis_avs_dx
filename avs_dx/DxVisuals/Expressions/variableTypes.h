#pragma once

// HLSL template engine uses these types in shader structures to bridge from HLSL to C++.

class float2 : public Vector2
{
public:
	float2( double x, double y ) : Vector2( (float)x, (float)y ) { }
};

class float3 : public Vector3
{
public:
	float3( double x, double y, double z ) : Vector3( (float)x, (float)y, (float)z ) { }

	void fromRgb( int rgb )
	{
		x = (float)( ( rgb >> 16 ) & 0xFF );
		y = (float)( ( rgb >> 8 ) & 0xFF );
		z = (float)( rgb & 0xFF );
		( *this ) *= ( 1.0f / 255.0f );
	}
};

class float4 : public Vector4
{
public:
	float4( double x, double y, double z, double w ) : Vector4( (float)x, (float)y, (float)z, (float)w ) { }
};

using uint2 = CSize;
using int2 = CPoint;
using uint = uint32_t;