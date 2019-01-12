#include "stdafx.h"
#include "ShaderBase.h"
#include <Hlsl/Compile/compile.h>
#include <Resources/createShader.hpp>
#include "setSizeDefines.h"

using namespace Hlsl;

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

void ShaderBase2::dropShader()
{
	CSLock __lock{ m_compiler.compilerLock() };
	shader = beatShader = nullptr;
	m_compiler.cancelPending();
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
	switch( m_state )
	{
	case eShaderState::Failed:
	case eShaderState::Pending:
		return S_FALSE;
	}

	setSizeDefines( hlsl, def );

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

	m_state = eShaderState::Pending;

	CSLock __lock( m_compiler.compilerLock() );
	if( shader && beatShader )
		return S_OK;
	return S_FALSE;
}

CComPtr<IUnknown> ShaderBase2::getShader( bool isbeat ) const
{
	if( m_state == eShaderState::Failed )
		return nullptr;

	if( m_state == eShaderState::Pending )
	{
		const eAsyncStatus as = m_compiler.asyncStatus();
		switch( as )
		{
		case eAsyncStatus::Failed:
			m_state = eShaderState::Failed;
			return nullptr;
		case eAsyncStatus::Completed:
			m_state = eShaderState::Good;
			break;
		}
		CSLock __lock{ m_compiler.compilerLock() };
		return isbeat ? beatShader : shader;
	}

	return isbeat ? beatShader : shader;
}

bool ShaderBase2::hasShader() const
{
	switch( m_state )
	{
	case eShaderState::Good:
		return true;
	case eShaderState::Pending:
		break;
	default:
		return false;
	}

	CSLock __lock{ m_compiler.compilerLock() };
	return shader && beatShader;
}

template<eStage stage>
bool ShaderBase<stage>::bind( bool isBeat ) const
{
	ShaderPtr<stage> s = getShader( isBeat );
	if( !s )
		return false;
	bindShader<shaderStage>( s );
	return true;
}

template<eStage stage>
ShaderPtr<stage> ShaderBase<stage>::getShader( bool isBeat ) const
{
	CComPtr<IUnknown> unk = __super::getShader( isBeat );
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