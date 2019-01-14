#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Expressions/CompiledShader.h>
#include "../Moves/Sampler.h"

using namespace Hlsl::Trans::Bump;

struct BumpStructs
{
	struct AvsState
	{
		int enabled;
		int depth;
		int depth2;
		int onbeat;
		uint32_t durFrames;
		int thisDepth;
		int blend;
		int blendavg;
		int nF;
		int codeHandle;
		int codeHandleBeat;
		int codeHandleInit;
		double *var_x;
		double *var_y;
		double *var_isBeat;
		double *var_isLongBeat;
		double *var_bi;
		RString code1, code2, code3;
		int need_recompile;
		int showlight;
		int initted;
		int invert;
		int AVS_EEL_CONTEXTNAME;
		int oldstyle;
		int buffern;
	};

	struct FixedStateData : public BumpState
	{
		FixedStateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 2; }
	};

	struct DynamicStateData : public Expressions::Compiler
	{
		DynamicStateData();
	};

	class StateData
	{
		FixedStateData m_fixed;
		DynamicStateData m_dynamic;

		vector<CStringA> m_templateGlobals;
		StateShaderTemplate m_template;

	public:

		StateData( const AvsState& s );

		HRESULT update( AvsState& ass );

		UINT stateSize() const;

		const StateShaderTemplate* shaderTemplate();

		HRESULT defines( Hlsl::Defines& def ) const;

		const Expressions::Compiler& compiler() const
		{
			return m_dynamic;
		}
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	using PsData = Hlsl::Trans::Bump::BumpPS;
};

class Bump : public EffectBase1<BumpStructs>
{
public:
	Bump( AvsState *pState );

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};