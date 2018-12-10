#pragma once
#include "../Effects/EffectImpl.hpp"

class EffectListBase : public EffectBase
{
public:
	struct T_RenderListType
	{
		C_RBASE *render;
		int effect_index;
		int has_rbase2;

		bool hasDxEffect() const
		{
			return render->dxEffect.operator bool();
		}
		EffectBase* dxEffect() const;
	};

	struct AvsState
	{
		int *thisfb;
		int l_w, l_h;
		int isroot;

		int num_renders, num_renders_alloc;
		T_RenderListType *renders;
		int inblendval, outblendval;
		int bufferin, bufferout;
		int ininvert, outinvert;

		int use_code;
		RString effect_exp[ 2 ];

		int inited;
		int codehandle[ 4 ];
		int need_recompile;
		CRITICAL_SECTION rcs;

		int AVS_EEL_CONTEXTNAME;
		double *var_beat, *var_alphain, *var_alphaout, *var_enabled, *var_clear, *var_w, *var_h;
		int isstart;

		int mode;

		int beat_render, beat_render_frames;
		int fake_enabled;
	};

	AvsState* const avs;
	EffectListBase( AvsState* pState ) : avs( pState ) { }

	HRESULT stateDeclarations( EffectStateBuilder &builder ) override { return E_NOTIMPL; }

	HRESULT updateParameters( Binder& binder ) override;

	HRESULT render( RenderTargets& rt ) override;

private:
	std::vector<EffectBase*> m_effects;

	template<class tFunc>
	HRESULT apply( tFunc fn )
	{
		HRESULT res = S_FALSE;
		for( auto p : m_effects )
		{
			const HRESULT hr = fn( *p );
			CHECK( hr );
			if( S_FALSE != hr )
				res = hr;
		}
		return res;
	}

protected:

	bool clearfb() const
	{
		return 0 != ( avs->mode & 1 );
	}

	// Collect renderers into the local vector. Return false if the list is unchanged, true if they were added, removed or reordered.
	bool updateList();

	bool updateState() override;
};