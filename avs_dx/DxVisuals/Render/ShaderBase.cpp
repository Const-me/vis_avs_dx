#include "stdafx.h"
#include "ShaderBase.h"
#include <Hlsl/compile.h>
#include <Resources/createShader.hpp>

extern int cfg_fs_d;

template<eStage stage>
HRESULT ShaderBase<stage>::compile( const char* name, const CStringA& hlsl, const CAtlMap<CStringA, CStringA>& inc, Hlsl::Defines &def, bool usesBeat, std::vector<uint8_t>& dxbc )
{
	if( eShaderState::Failed == m_state )
		return S_FALSE;

	dropShader();
	m_state = eShaderState::Failed;

	bool shaderNeedsResizeEvent = false;
	if( hlsl.Find( "AVS_RENDER_SIZE" ) >= 0 )
	{
		def.set( "AVS_RENDER_SIZE", getRenderSizeString() );
		shaderNeedsResizeEvent = true;
	}
	if( hlsl.Find( "AVS_PIXEL_PARTICLES" ) >= 0 )
	{
		def.set( "AVS_PIXEL_PARTICLES", cfg_fs_d ? "1" : "0" );
		shaderNeedsResizeEvent = true;
	}
	if( shaderNeedsResizeEvent )
		subscribeHandler( this );
	else
		unsubscribeHandler( this );

	if( usesBeat )
		def.set( "IS_BEAT", "0" );

	// Compile HLSL into DXBC
	CHECK( Hlsl::compile( shaderStage, hlsl, name, inc, def, dxbc ) );

	// Upload DXBC to GPU
	CHECK( createShader( dxbc, shader ) );

	if( usesBeat )
	{
		def.reset( "IS_BEAT", "1" );
		CHECK( Hlsl::compile( shaderStage, hlsl, name, inc, def, dxbc ) );
		CHECK( createShader( dxbc, beatShader ) );
	}
	else
		beatShader = shader;

	m_state = eShaderState::Good;
	return S_OK;
}

template<eStage stage>
bool ShaderBase<stage>::bind( bool isBeat ) const
{
	const ShaderPtr<shaderStage>& s = isBeat ? beatShader : shader;
	if( nullptr == s )
		return false;
	bindShader<shaderStage>( s );
	return true;
}

template<eStage stage>
bool ShaderBase<stage>::hasShader() const
{
	return m_state == eShaderState::Good;
}

template<eStage stage>
void ShaderBase<stage>::dropShader()
{
	shader = beatShader = nullptr;
	m_state = eShaderState::Updated;
}

template<eStage stage>
IShader<stage> *ShaderBase<stage>::ptr( bool isBeat ) const
{
	return isBeat ? beatShader : shader;
}

template<eStage stage>
void ShaderBase<stage>::onRenderSizeChanged()
{
	dropShader();
}

template class ShaderBase<eStage::Compute>;
template class ShaderBase<eStage::Vertex>;
template class ShaderBase<eStage::Geometry>;
template class ShaderBase<eStage::Pixel>;