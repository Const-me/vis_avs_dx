#pragma once
#include "eVarType.h"

namespace Expressions
{
	class Prototype
	{
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

		CStringA initState() const;

		CStringA stateLoad() const;

		CStringA stateStore() const;

		std::vector<sInputOutput> inputs, outputs;
	};
}