#pragma once
#include "Compiler.h"

namespace Expressions
{
	// A shader template with SHADER_GLOBALS & SHADER_CODE placeholder to be filled by the expression compiler.
	template<class T>
	class CompiledShader : public T
	{
	public:
		CompiledShader() :
			m_template( *T::shaderTemplate() ),
			origTemplate( m_template.hlsl )
		{ }

		HRESULT updateDx( const Expressions::Compiler& compiler )
		{
			if( compiler.fragmentCode() == fragmentCode && compiler.fragmentGlobals() == fragmentGlobals )
				return S_FALSE;	// The compiler has the same code, nothing to update

			fragmentCode = compiler.fragmentCode();
			fragmentGlobals = compiler.fragmentGlobals();
			m_template.hlsl = origTemplate;
			// TODO: cache positions
			m_template.hlsl.Replace( "SHADER_GLOBALS", fragmentGlobals );
			m_template.hlsl.Replace( "SHADER_CODE", fragmentCode );
			return S_OK;
		}

		const ShaderTemplate* shaderTemplate() { return &m_template; }

	private:
		ShaderTemplate m_template;
		// The original HLSL code of the template, with placeholders intact.
		const CStringA origTemplate;
		// Copy of the code produced by Expressions::Compiler class. That code will go into the placeholders.
		CStringA fragmentCode, fragmentGlobals;
	};
}