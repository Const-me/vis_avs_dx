#pragma once
#include "../Hlsl/ShaderTemplate.h"
#include "Compiler.h"

namespace Expressions
{
	// Non-template base class to save compilation time and code size
	class CompiledShaderBase
	{
		ShaderTemplate m_template;
		std::array<CStringA, 3> templatePieces;
		// Copy of the code produced by Expressions::Compiler class. That code will go into the placeholders.
		CStringA fragmentCode, fragmentGlobals;
		bool needsRng = false;

		void assembleShader();

	public:

		CompiledShaderBase( const ShaderTemplate* origTemplate );

	protected:

		const ShaderTemplate* compiledTemplate() const
		{
			return &m_template;
		}

		HRESULT updateCode( const Expressions::Compiler& compiler );

		void compiledDefines( Hlsl::Defines& def ) const;
	};

	// A shader template with SHADER_GLOBALS & SHADER_CODE placeholders to be filled by the expression compiler.
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
			__if_exists( T::defines )
			{
				compiledDefines( def );
				return T::defines( def );
			}
			__if_not_exists( T::defines )
			{
				compiledDefines( def );
				return S_FALSE;
			}
		}

		HRESULT updateDx( const Expressions::Compiler& compiler )
		{
			return updateCode( compiler );
		}
	};
}