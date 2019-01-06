#pragma once
#include "../../EffectImpl.hpp"
#include "../../../Expressions/CompiledShader.h"
#include "../PointSpritesRender.hpp"
#include <variant>
using namespace Hlsl::Render::SuperScope;

struct ScopeBase
{
public:
	struct AvsState
	{
		RString effect_exp[ 4 ];
		int which_ch;
		int num_colors;
		int colors[ 16 ];
		int mode;

		bool drawingLines() const;

		float sampleV() const;
	};

	struct FixedStateData : public SuperScopeState
	{
		FixedStateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 4; }
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

		CSize screenSize;
		bool drawingLines;

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

	struct VsData : public Expressions::CompiledShader<SuperScopeVS>
	{
		HRESULT updateAvs( const AvsState& avs );

		HRESULT updateDx( const StateData& sd )
		{
			return CompiledShader::updateDx( sd.compiler() );
		}

		HRESULT defines( Hlsl::Defines& def );
	};

	struct DotRendering
	{
		Shader<Hlsl::Render::PointSpriteGS> gs;
		Shader<Hlsl::Render::PointSpritePS> ps;
	};

	struct LinesRendering
	{
		Shader<Hlsl::Render::PolylineGS> gs;
		Shader<Hlsl::Render::PolylinePS> ps;
	};
};

class SuperScope : public EffectBase1<ScopeBase>
{
	std::variant<DotRendering, LinesRendering> m_render;

public:
	SuperScope( AvsState *pState ) : EffectBase1( pState ) { }

	DECLARE_EFFECT()

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};