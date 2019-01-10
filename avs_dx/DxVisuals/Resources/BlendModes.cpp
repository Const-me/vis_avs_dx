#include "stdafx.h"
#include "BlendModes.h"
#include "RenderTargets.h"

extern int g_line_blend_mode;

inline float adjastableBlendValue()
{
	const int v = g_line_blend_mode >> 8;
	return (float)( v & 0xFF ) * div255;
}

float BlendModes::getLineWidth()
{
	const int v = g_line_blend_mode >> 16;
	return (float)( v * 0xFF );
}

bool BlendModes::setupBlending( RenderTargets& rt )
{
	const int mode = g_line_blend_mode & 0xFF;

	if( mode != 8 )
		rt.writeToLast( false );

	switch( mode )
	{
	case 0:
		omBlend( eBlend::None );
		return true;
	case 1:
		omBlend( eBlend::Add );
		return true;
	case 2:
		return setCustom( eCustom::Maximum );
	case 3:
		omCustomBlend( 0.5f );
		return true;
	case 4:
		return setCustom( eCustom::Sub1 );
	case 5:
		return setCustom( eCustom::Sub2 );
	case 6:
		return setCustom( eCustom::Multiply );
	case 7:
		omCustomBlend( adjastableBlendValue() );
		return true;
	case 8:
		if( setXorBlend() )
		{
			rt.writeToLastWithLogicOp();
			return true;
		}
		rt.writeToLast( false );
		return false;
	case 9:
		return setCustom( eCustom::Minimum );
	}
	return false;
}

inline bool setBlend( ID3D11BlendState *bs )
{
	context->OMSetBlendState( bs, nullptr, 0xffffffff );
	return true;
}

HRESULT createXorState( CComPtr<ID3D11BlendState1> &bs )
{
	CComPtr<ID3D11Device1> device1;
	CHECK( device->QueryInterface( IID_PPV_ARGS( &device1 ) ) );

	CD3D11_BLEND_DESC1 bd1{ D3D11_DEFAULT };
	D3D11_RENDER_TARGET_BLEND_DESC1& rt = bd1.RenderTarget[ 0 ];
	rt.LogicOpEnable = TRUE;
	rt.LogicOp = D3D11_LOGIC_OP_XOR;
	rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	CHECK( device1->CreateBlendState1( &bd1, &bs ) );
	return S_OK;
}

bool BlendModes::setXorBlend()
{
	if( m_xorMode )
		return setBlend( m_xorMode );

	if( getFeatureLevel() < D3D_FEATURE_LEVEL_11_1 )
		return false;
	static bool s_failed = false;
	if( s_failed )
		return false;

	if( FAILED( createXorState( m_xorMode ) ) )
	{
		s_failed = true;
		return false;
	}
	return setBlend( m_xorMode );
}

bool BlendModes::setCustom( eCustom mode )
{
	sState& bs = m_customModex[ (uint8_t)mode ];
	if( bs.bs )
		return setBlend( bs.bs );

	if( bs.failed )
		return false;

	CD3D11_BLEND_DESC bd{ D3D11_DEFAULT };
	D3D11_RENDER_TARGET_BLEND_DESC& rt = bd.RenderTarget[ 0 ];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D11_BLEND_ONE;
	rt.DestBlend = D3D11_BLEND_ONE;

	switch( mode )
	{
	case eCustom::Maximum:
		rt.BlendOp = D3D11_BLEND_OP_MAX;
		break;
	case eCustom::Minimum:
		rt.BlendOp = D3D11_BLEND_OP_MIN;
		break;
	case eCustom::Sub1:
		rt.BlendOp = D3D11_BLEND_OP_SUBTRACT;
		break;
	case eCustom::Sub2:
		rt.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		break;
	case eCustom::Multiply:
		rt.BlendOp = D3D11_BLEND_OP_ADD;
		rt.SrcBlend = D3D11_BLEND_DEST_COLOR;
		rt.DestBlend = D3D11_BLEND_ZERO;
	}

	if( FAILED( device->CreateBlendState( &bd, &bs.bs ) ) )
	{
		bs.failed = true;
		return false;
	}

	return setBlend( bs.bs );
}

void BlendModes::destroy()
{
	for( sState& s : m_customModex )
	{
		s.bs = nullptr;
		s.failed = false;
	}
	m_xorMode = nullptr;
}