#include "stdafx.h"
#include "CompiledShader.h"

namespace
{
	std::array<CStringA, 3> splitTemplate( const CStringA& hlsl )
	{
		const CStringA macroGlobals = "SHADER_GLOBALS";
		const CStringA macroCode = "SHADER_CODE";

		const int sg = hlsl.Find( macroGlobals );
		const int mc = hlsl.Find( macroCode, sg );
		if( sg < 0 || mc < 0 )
		{
			logError( "The template doesn't contain SHADER_GLOBALS and SHADER_CODE macros" );
			__debugbreak();
		}

		const int sge = sg + macroGlobals.GetLength();
		const int mce = mc + macroCode.GetLength();

		std::array<CStringA, 3> res;
		res[ 0 ] = hlsl.Left( sg );
		res[ 1 ] = hlsl.Mid( sge, mc - sge );
		res[ 2 ] = hlsl.Mid( mce );
		return std::move( res );
	}
}

using namespace Expressions;

CompiledShaderBase::CompiledShaderBase( const ShaderTemplate* origTemplate ) :
	m_template( *origTemplate ),
	templatePieces( splitTemplate( m_template.hlsl ) )
{ }

HRESULT CompiledShaderBase::updateDx( const Expressions::Compiler& compiler )
{
	if( compiler.fragmentCode() == fragmentCode && compiler.fragmentGlobals() == fragmentGlobals )
		return S_FALSE;	// The compiler has the same code, nothing to update

	fragmentCode = compiler.fragmentCode();
	fragmentGlobals = compiler.fragmentGlobals();

	m_template.hlsl = templatePieces[ 0 ];
	m_template.hlsl += fragmentGlobals;
	m_template.hlsl += templatePieces[ 1 ];
	m_template.hlsl += fragmentCode;
	m_template.hlsl += templatePieces[ 2 ];

	needsRng = compiler.fragmentUsesRng();
	m_template.usesBeat = compiler.fragmentUsesBeat();
	return S_OK;
}

void CompiledShaderBase::compiledDefines( Hlsl::Defines& def ) const
{
	if( needsRng )
		def.set( "NEEDS_RNG", "1" );
}