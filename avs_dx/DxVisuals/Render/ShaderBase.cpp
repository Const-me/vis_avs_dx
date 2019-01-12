#include "stdafx.h"
#include "ShaderBase.h"
#include <Hlsl/Compile/compile.h>
#include <Resources/createShader.hpp>
#include "setSizeDefines.h"

void ShaderBase2::setSizeDefines( const CStringA& hlsl, Hlsl::Defines &def )
{
	::setSizeDefines( hlsl, def, this );
}

ShaderBase2::ShaderBase2( eStage stage, const CAtlMap<CStringA, CStringA>& includes ) :
	m_compiler( stage, includes )
{ }

ShaderBase2::~ShaderBase2()
{
	unsubscribeHandler( this );
	m_compiler.shutdownWorker();
}

CComPtr<IUnknown> ShaderBase2::getShader( bool isbeat ) const
{
	CSLock __lock{ m_compiler.compilerLock() };
	return isbeat ? beatShader : shader;
}

bool ShaderBase2::hasShader() const
{
	return m_state == eShaderState::Good;
}

void ShaderBase2::dropShader()
{
	CSLock __lock{ m_compiler.compilerLock() };
	shader = beatShader = nullptr;
	m_state = eShaderState::Updated;
}

void ShaderBase2::onRenderSizeChanged()
{
	dropShader();
}

namespace
{
	template<bool isBeat>
	static void setBeatMacro( Hlsl::Defines &def )
	{
		def.set( "IS_BEAT", isBeat ? "1" : "0" );
	}
}

HRESULT ShaderBase2::compile( const char* name, const CStringA& hlsl, Hlsl::Defines &def, bool usesBeat, Hlsl::pfnCompiledShader pfnCompiled, void* compiledArg )
{
	if( eShaderState::Failed == m_state )
		return S_FALSE;

	setSizeDefines( hlsl, def );
	using namespace Hlsl;

	if( usesBeat )
	{
		Job jobs[ 2 ] =
		{
			Job{ name, &shader.p, &setBeatMacro<false> },
			Job{ name, &beatShader.p, &setBeatMacro<true> },
		};
		jobs[ 0 ].fnCompiledShader = pfnCompiled;
		jobs[ 0 ].compiledShaderContext = compiledArg;
		m_compiler.submit( hlsl, def, jobs, 2 );
	}
	else
	{
		Job job{ name, &shader.p, &beatShader.p };
		job.fnCompiledShader = pfnCompiled;
		job.compiledShaderContext = compiledArg;
		m_compiler.submit( hlsl, def, &job, 1 );
	}

	{
		CSLock __lock( m_compiler.compilerLock() );
		if( shader && beatShader )
		{
			// We still have the old shaders. Use them for a couple of frames while the new ones are being compiled.
			m_state = eShaderState::Good;
			return S_OK;
		}
	}

	m_compiler.join();
	const eAsyncStatus as = m_compiler.asyncStatus();
	if( as == eAsyncStatus::Failed )
	{
		m_state = eShaderState::Failed;
		return E_FAIL;
	}

	m_state = eShaderState::Good;
	return S_OK;
}

template<eStage stage>
bool ShaderBase<stage>::bind( bool isBeat ) const
{
	ShaderPtr<stage> s = ptr( isBeat );
	if( !s )
		return false;
	bindShader<shaderStage>( s );
	return true;
}

template<eStage stage>
ShaderPtr<stage> ShaderBase<stage>::ptr( bool isBeat ) const
{
	CComPtr<IUnknown> unk = getShader( isBeat );
	if( nullptr == unk )
		return nullptr;

	ShaderPtr<stage> res;
	res.Attach( static_cast<IShader<stage> *>( unk.Detach() ) );
	return eastl::move( res );
}

template class ShaderBase<eStage::Compute>;
template class ShaderBase<eStage::Vertex>;
template class ShaderBase<eStage::Geometry>;
template class ShaderBase<eStage::Pixel>;