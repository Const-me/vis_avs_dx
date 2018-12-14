#pragma once
#include "../Effects/includeDefs.h"
#include "../Render/EffectStateShader.hpp"
#include "Prototype.h"

namespace Expressions
{
	class Compiler
	{
	public:


		// Reserve a fixed state variable
		HRESULT addState( const CStringA& name, uint32_t def );
		HRESULT addState( const CStringA& name, float def );

		// Declare input constant usable by all expressions. Will become a macro.
		HRESULT addInput( const CStringA& name, eVarType vt );
		// Declare output variable produced by the fragment expression.
		HRESULT addOutput( const CStringA& name, eVarType vt = eVarType::f32 );

		// Recompile stuff from the strings
		HRESULT update( RString effect_exp[ 4 ] );

		// Rebuild the state shader
		HRESULT buildState( EffectStateShader& ess );

	private:

		struct sFixedStateVar
		{
			CStringA name;
			CStringA initVal;
			eVarType vt;
			int offset = -1;
		};
		std::vector<sFixedStateVar> m_fixedState;

		enum eExpressionBits : uint8_t
		{
			None = 0,
			Init = 1,
			Frame = 2,
			Beat = 4,
			Fragment = 8,
		};

		// Local copy of the effect's expression strings
		std::array<CStringA, 4> m_expressions;

		// Global functions / resource declarations used by the state shaders.
		CAtlMap<CStringA, bool> m_globalsCode;

		struct sVariable
		{
			// Name of the variable from the user's expressions
			CStringA name;
			// Offset inside effectStates, from the start of this effect's state block, in 32-bit elements. -1 indicates local variables which that don't need to hit the state buffer.
			int offset = -1;

			eVarType vt = eVarType::unknown;

			// Combination of bits that says where it's used.
			eExpressionBits usageMask = eExpressionBits::None;
		};

		CStringA m_hlslState;
		CStringA m_hlslFragment;

		std::vector<sVariable> m_vars;

		HRESULT parse( const CStringA& exp, uint8_t what, CStringA& hlsl );

		HRESULT deductTypes();
	};
}