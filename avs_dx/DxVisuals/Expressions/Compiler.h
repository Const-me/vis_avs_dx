#pragma once
#include "../Effects/includeDefs.h"
#include "../Render/EffectStateShader.hpp"
#include "Prototype.h"
#include "Tree/Tree.h"

namespace Expressions
{
	class Compiler
	{
		const Prototype& proto;

	public:
		Compiler( const char* effectName, const Prototype& effectPrototype );
		Compiler( const Compiler & ) = delete;

		// Recompile stuff from the strings
		HRESULT update( RString effect_exp[ 4 ] );

		const StateShaderTemplate* shaderTemplate() { return &m_stateTemplate; }

		UINT stateSize() const { return (UINT)m_stateSize; }

		const CStringA& fragmentGlobals() const { return m_fragmentGlobals; }

		const CStringA& fragmentCode() const { return m_hlslFragment; }

	protected:
		template<class T>
		inline HRESULT updateInput( T& oldVal, const T& newVal )
		{
			if( oldVal == newVal )
				return S_FALSE;
			oldVal = newVal;
			return S_OK;
		}

	private:
		// Local copy of the effect's expression strings
		std::array<CStringA, 4> m_expressions;
		// Recompiled from NSEEL to HLSL
		std::array<CStringA, 4> m_hlsl;

		// Symbols table. The VariablesTable accumulates variables from all 4 expressions, FunctionsTable is temporary.
		SymbolTable m_symbols;
		Tree m_tree;

		// Variable usage by all 3 state expressions combined
		std::vector<eVarAccess> m_stateUsage;
		// Variables usage by the fragment expression.
		std::vector<eVarAccess> m_fragmentUsage;
		
		int m_stateSize;

		CStringA m_fragmentGlobals;
		CStringA m_hlslFragment;
		std::vector<CStringA> m_stateGlobals;
		StateShaderTemplate m_stateTemplate;
	};
}