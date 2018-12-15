#pragma once
#include "../Effects/includeDefs.h"
#include "../Render/EffectStateShader.hpp"
#include "Prototype.h"
#include "../Hlsl/Defines.h"
#include "parse.h"

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

		struct sVariable
		{
			// Name of the variable from the user's expressions
			CStringA name;
			// Offset inside effectStates, from the start of this effect's state block, in 32-bit elements. -1 indicates local variables which that don't need to hit the state buffer.
			int offset = -1;
			eVarType vt = eVarType::unknown;
			bool usedInState;
			bool usedInFragment;
		};
		std::vector<sVariable> m_vars;
		int m_stateSize;

		CStringA m_fragmentGlobals;
		CStringA m_hlslFragment;
		std::vector<CStringA> m_stateGlobals;
		StateShaderTemplate m_stateTemplate;

		HRESULT allocVariables( const std::array<Assignments, 4>& parsed );

		static void printAssignments( CStringA& code, const Assignments& ass );
		void printLoadState( CStringA& code ) const;
		void printStoreState( CStringA& code ) const;

		HRESULT buildStateCode( const std::array<Assignments, 4>& parsed );
		HRESULT buildFragmentCode( const Assignments& parsed );
	};
}