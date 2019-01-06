#pragma once
#include "VariableDecl.hpp"
#include "variableTypes.h"

namespace Expressions
{
	// A prototype for the expression-based effect, contains declarations of input/output variables/constants visible to the scripts code.
	class Prototype
	{
		// Count of uint variables in the state block
		int m_size = 0;

		HRESULT addState( const CStringA& name, eVarType vt, const CStringA& initVal );

		struct FixedStateVar : public VariableDecl
		{
			CStringA initVal;
			int offset = -1;
			FixedStateVar() = default;
			FixedStateVar( eVarType t, const CStringA& n, const CStringA& iv, int off ) :
				VariableDecl( eVarLocation::stateStatic, t, n ), initVal( iv ), offset( off ) { }
		};

		std::vector<FixedStateVar> m_fixedState;

		std::vector<VariableDecl> m_vars;
		CStringA m_beatMacro;

		HRESULT addVariable( eVarLocation loc, eVarType vt, const CStringA& name );

		struct IndirectDrawArgs
		{
			CStringA name;
			uint4 init;
			CStringA update;
			int offset;
		};
		std::vector<IndirectDrawArgs> m_indirectArgs;

	public:

		// Statically allocate `uint` state variable. State variables are read/write in state shader, read-only in fragment expression.
		HRESULT addState( const CStringA& name, uint32_t def );

		// Statically allocate `float` state variable. State variables are read/write in state shader, read-only in fragment expression.
		HRESULT addState( const CStringA& name, float def );

		// Declare input constant usable by all expressions. Will become a macro.
		HRESULT addConstantInput( const CStringA& name, eVarType vt )
		{
			return addVariable( eVarLocation::macro, vt, name );
		}

		// Statically allocate uint4 state variable to be used with DrawInstancedIndirect API. Will not be used by NSEEL code.
		HRESULT addIndirectDrawArgs( const CStringA& name, const uint4& init, const char* update );

		// Declare variable that IS_BEAT macro will go to. If the effect will actually use this, it will cause 2 versions of the shader to be compiled, saving per-pixel branching in runtime.
		HRESULT addBeatConstant( const CStringA& name );

		// Declare input variable used by fragment expression, read-only.
		HRESULT addFragmentInput( const CStringA& name, eVarType vt = eVarType::f32 )
		{
			return addVariable( eVarLocation::fragmentInput, vt, name );
		}

		// Declare output variable produced by fragment expression, read/write 
		HRESULT addFragmentOutput( const CStringA& name, eVarType vt = eVarType::f32 )
		{
			return addVariable( eVarLocation::fragmentOutput, vt, name );
		}

		int fixedStateSize() const { return m_size; }

		// HLSL piece for state initialization
		CStringA initState() const;

		// HLSL piece for loading state variables from the untyped buffer
		CStringA stateLoad( int offset = 0 ) const;

		// HLSL piece for storing state variables
		CStringA stateStore( int offset = 0 ) const;

		template<class TFunc>
		void enumVariables( TFunc fn ) const
		{
			for( const auto& fs : m_fixedState )
				fn( fs );
			for( const auto& v : m_vars )
				fn( v );
		}

		template<class TFunc>
		void enumMacros( TFunc fn ) const
		{
			for( const auto& v : m_vars )
				if( v.loc == eVarLocation::macro )
					fn( v );
		}

		const CStringA& getBeatMacro() const
		{
			return m_beatMacro;
		}

		int getIndirectArgOffset( int index ) const
		{
			return m_indirectArgs[ index ].offset;
		}
	};
}