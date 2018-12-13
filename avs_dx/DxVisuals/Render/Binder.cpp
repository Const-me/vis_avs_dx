#include "stdafx.h"
#include "Binder.h"

Binder::Binder()
{
	ZeroMemory( &m_data, sizeof( m_data ) );
	
	for( Slots& s : m_data )
	{
		// cbuffer FrameGlobalData : register(b0)
		s.cbuffer++;

		// Texture2D<float> texVisData : register(t0);
		s.srv++;

		// ByteAddressBuffer effectStates : register(t1);
		s.srv++;
	}

	// RWByteAddressBuffer effectStates : register(u0);
	reserveInputSlot( eStage::Compute, 'u' );

	// Texture2D<float4> prevFrameTexture : register(t2);
	reserveInputSlot( eStage::Pixel, 't' );
}

namespace
{
	inline UINT inc( UINT& v, UINT limit )
	{
		v++;
		return std::min( v, limit - 1 );
	}
}

// Reserve shader slot for the specified resource, return string like "t4" that can be compiled into the shader code to access that resource
UINT Binder::reserveInputSlot( eStage pipelineStage, char resourceType )
{
	Slots& s = m_data[ (uint8_t)pipelineStage ];
	switch( resourceType )
	{
	case 't':
		return inc( s.srv, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT );
	case 'u':
		return inc( s.uav, D3D11_PS_CS_UAV_REGISTER_COUNT );
	case 'b':
		return inc( s.cbuffer, D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT );
	}
	__debugbreak();
	return 0;
}