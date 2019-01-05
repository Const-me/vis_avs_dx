#include "stdafx.h"
#include "Binder.h"

Binder::Binder()
{
	ZeroMemory( &m_data, sizeof( m_data ) );
	
	for( Slots& s : m_data )
	{
		// cbuffer FrameGlobalData : register(b0)
		s.cbuffer++;

		// Texture2D<float> texVisDataU8 : register(t0);
		s.srv++;

		// Texture2D<float> texVisDataS8 : register(t1);
		s.srv++;

		// ByteAddressBuffer effectStates : register(t2);
		s.srv++;

		// SamplerState sampleBilinear : register(s0);
		s.sampler++;

		// SamplerState sampleNearest : register(s1);
		s.sampler++;
	}

	// RWByteAddressBuffer effectStates : register(u0);
	UINT unused;
	reserveInputSlot( unused, eStage::Compute, 'u' );

	// Texture2D<float4> prevFrameTexture : register(t2);
	reserveInputSlot( unused, eStage::Pixel, 't' );
}

namespace
{
	inline bool inc( UINT& v, UINT& result, UINT limit )
	{
		v++;
		const UINT newResult = std::min( v, limit - 1 );
		const bool changed = newResult != result;
		result = newResult;
		return changed;
	}
}

// Reserve shader slot for the specified resource, return string like "t4" that can be compiled into the shader code to access that resource
bool Binder::reserveInputSlot( UINT& result, eStage pipelineStage, char resourceType )
{
	Slots& s = m_data[ (uint8_t)pipelineStage ];
	switch( resourceType )
	{
	case 't':
		return inc( s.srv, result, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT - 2 );	// The 2 last ones are used by Transition
	case 'u':
		return inc( s.uav, result, D3D11_PS_CS_UAV_REGISTER_COUNT );
	case 'b':
		return inc( s.cbuffer, result, D3D11_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT );
	case 's':
		return inc( s.sampler, result, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT );
	}
	__debugbreak();
	return 0;
}