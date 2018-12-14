#pragma once
#include "eVarType.h"

namespace Expressions
{
	// A prototype for the expression-based effect, contains input/output variables/constants.
	class Prototype
	{
		// Count if uint variables in the state block
		int m_size = 0;

		HRESULT addState( const CStringA& name, eVarType vt, const CStringA& initVal );

		struct sInputOutput
		{
			CStringA name;
			eVarType vt;
		};

		static HRESULT addInputOutput( const CStringA& name, eVarType vt, std::vector<sInputOutput>& vec );

		struct sFixedStateVar
		{
			CStringA name;
			CStringA initVal;
			eVarType vt;
			int offset = -1;
		};

		std::vector<sFixedStateVar> m_fixedState;

	public:

		HRESULT addState( const CStringA& name, uint32_t def );

		HRESULT addState( const CStringA& name, float def );

		// Declare input constant usable by all expressions. Will become a macro.
		HRESULT addInput( const CStringA& name, eVarType vt )
		{
			return addInputOutput( name, vt, inputs );
		}
		// Declare output variable produced by the fragment expression.
		HRESULT addOutput( const CStringA& name, eVarType vt = eVarType::f32 )
		{
			return addInputOutput( name, vt, outputs );
		}

		int fixedStateSize() const { return m_size; }

		// HLSL piece for state initialization
		CStringA initState() const;

		// HLSL piece for loading state variables from the untyped buffer
		CStringA stateLoad() const;

		// HLSL piece for storing state variables
		CStringA stateStore() const;

		std::vector<sInputOutput> inputs, outputs;

		template<class TFunc>
		void enumBuiltins( TFunc fn ) const
		{
			for( const auto& fs : m_fixedState )
				fn( fs.name, fs.vt );
			for( const auto& i : inputs )
				fn( i.name, i.vt );
			for( const auto& o : outputs )
				fn( o.name, o.vt );
		}
	};
}