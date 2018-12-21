#pragma once
#include "VariableDecl.hpp"

namespace Expressions
{
	// A prototype for the expression-based effect, contains input/output variables/constants.
	class Prototype
	{
		// Count of uint variables in the state block
		int m_size = 0;

		HRESULT addState( const CStringA& name, eVarType vt, const CStringA& initVal );

		struct FixedStateVar: public VariableDecl
		{
			CStringA initVal;
			int offset = -1;
			FixedStateVar() = default;
			FixedStateVar( eVarType t, const CStringA& n, const CStringA& iv, int off ) :
				VariableDecl( eVarLocation::stateStatic, t, n ), initVal( iv ), offset( off ) { }
		};

		std::vector<FixedStateVar> m_fixedState;

		std::vector<VariableDecl> m_vars;

		HRESULT addVariable( eVarLocation loc, eVarType vt, const CStringA& name );

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

		// Declare input variable used by fragment expression, read-only.
		HRESULT addFragmentInput( const CStringA& name, eVarType vt = eVarType::f32 )
		{
			return addVariable( eVarLocation::fragmentInput, vt, name);
		}

		// Declare output variable produced by fragment expression, read/write 
		HRESULT addFragmentOutput( const CStringA& name, eVarType vt = eVarType::f32 )
		{
			return addVariable( eVarLocation::fragmentOutput, vt, name);
		}

		int fixedStateSize() const { return m_size; }

		// HLSL piece for state initialization
		CStringA initState() const;

		// HLSL piece for loading state variables from the untyped buffer
		CStringA stateLoad() const;

		// HLSL piece for storing state variables
		CStringA stateStore() const;

		template<class TFunc>
		void enumVariables( TFunc fn ) const
		{
			for( const auto& fs : m_fixedState )
				fn( fs );
			for( const auto& v : m_vars )
				fn( v );
		}
	};
}