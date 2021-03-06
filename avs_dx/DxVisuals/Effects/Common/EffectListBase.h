#pragma once
#include "EffectImpl.hpp"

class RootEffect;

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
			return nullptr != getDxEffect( render );
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
		int mode;

		int beat_render, beat_render_frames;
		int fake_enabled;
	};

	AvsState* const avs;
	EffectListBase( AvsState* pState );

	HRESULT updateParameters( Binder& binder ) override;

	void bindResources() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;

	vector<EffectBase*> m_effects;

	virtual HRESULT clearRenders() { return S_FALSE; }

private:
	EffectProfiler m_profilerStart;

	template<class tFunc>
	HRESULT apply( tFunc fn )
	{
		BoolHr hr;
		for( auto p : m_effects )
			hr.combine( fn( *p ) );
		return hr;
	}

protected:
	bool clearfb() const
	{
		return 0 != ( avs->mode & 1 );
	}

	// Collect renderers into the local vector. Return false if the list is unchanged, true if they were added, removed or reordered.
	bool updateList( RootEffect* pRoot );

	HRESULT shouldRebuildState() override;

	// Call the function for any non-list effect in the hierarchy, walking the nested lists in depth first order.
	template<class tFunc>
	HRESULT applyRecursively( tFunc fn ) const
	{
		for( EffectBase* eb : m_effects )
		{
			if( eb->metadata().isList )
			{
				EffectListBase* pList = static_cast<EffectListBase*>( eb );
				CHECK( pList->applyRecursively( fn ) );
			}
			else
			{
				CHECK( fn( *eb ) );
			}
		}
		return S_OK;
	}

	void clearEffects()
	{
		m_effects.clear();
	}

	static HRESULT fadeRenderTarget( RenderTargets &rt );
};