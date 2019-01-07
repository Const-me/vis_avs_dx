#pragma once
#include "../Render/EffectStateShader.hpp"
#include "Prototype.h"
#include "Tree/Tree.h"

namespace Expressions
{
	class Compiler
	{
		const Prototype& proto;
		const int m_stateOffset;

	public:
		Compiler( const char* effectName, const Prototype& effectPrototype, UINT stateOffset = 0 );
		Compiler( const Compiler & ) = delete;

		static HRESULT defines( Hlsl::Defines& def ) { return S_FALSE; };

		// Recompile stuff from the strings
		HRESULT update( const char* init, const char* frame, const char* beat, const char* fragment );

		const StateShaderTemplate* shaderTemplate() { return &m_stateTemplate; }

		UINT stateSize() const { return (UINT)m_stateSize; }

		const CStringA& fragmentGlobals() const { return m_fragmentGlobals; }

		bool fragmentUsesRng() const { return m_fragmentRng; }
		bool fragmentUsesBeat() const { return m_fragmentBeat; }
		const CStringA& fragmentCode() const { return m_hlslFragment; }

		int getIndirectArgOffset( int index = 0 ) const;

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
		array<CStringA, 4> m_expressions;
		// Recompiled from NSEEL to HLSL
		array<CStringA, 4> m_hlsl;

		// Symbols table. The VariablesTable accumulates variables from all 4 expressions, FunctionsTable is temporary.
		SymbolTable m_symbols;
		Tree m_tree;

		vector<VarUseFlags> m_varUsage;
		CStringA m_dynStateLoad, m_dynStateStore;
		
		int m_stateSize;
		bool m_fragmentRng;
		bool m_fragmentBeat;

		CStringA m_fragmentGlobals;
		CStringA m_hlslFragment;
		vector<CStringA> m_stateGlobals;
		StateShaderTemplate m_stateTemplate;

		void appendMacros( CStringA& hlsl ) const;
		void allocateState();
		void buildStateHlsl();
		void buildFragmentHlsl();
	};

	CStringA expressionMacroName( const char* name );

	// Built-in effects use variable names like `w` and `x`, defining such short symbols breaks some #included HLSL code.
	// That's why renaming the defines to e.g. `MACRO_w`. The script engine still sees short names, the longer name is for HLSL only.
	template<class T>
	inline void setExpressionMacro( Hlsl::Defines& def, const char* name, T value )
	{
		def.set( expressionMacroName( name ), value );
	}
}