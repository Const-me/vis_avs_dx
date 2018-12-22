#pragma once
#include "../Hlsl/ShaderTemplate.h"
#include "Compiler.h"

namespace Expressions
{
	class CompiledShaderBase
	{
		ShaderTemplate m_template;
		std::array<CStringA, 3> templatePieces;
		// Copy of the code produced by Expressions::Compiler class. That code will go into the placeholders.
		CStringA fragmentCode, fragmentGlobals;
		bool needsRng = false;

	public:

		CompiledShaderBase( const ShaderTemplate* origTemplate );

		HRESULT updateDx( const Expressions::Compiler& compiler );

	protected:

		const ShaderTemplate* compiledTemplate() const
		{
			return &m_template;
		}

		void compiledDefines( Hlsl::Defines& def ) const;
	};

	// A shader template with SHADER_GLOBALS & SHADER_CODE placeholder to be filled by the expression compiler.
	template<class T>
	class CompiledShader : public T, public CompiledShaderBase
	{
	public:
		CompiledShader() : CompiledShaderBase( T::shaderTemplate() ) { }

		const ShaderTemplate* shaderTemplate() const
		{
			return compiledTemplate();
		}

		HRESULT defines( Hlsl::Defines& def ) const
		{
			compiledDefines( def );
			return T::defines( def );
		}
	};
}