#pragma once
#include "EffectStateShader.hpp"
#include <Hlsl/Async/CompileWorker.h>
#include <Utils/resizeHandler.h>

class StateShaders: public iResizeHandler
{
	Hlsl::CompileWorker<1> m_initCompiler;
	Hlsl::CompileWorker<2> m_updateCompiler;

	CComPtr<ID3D11ComputeShader> m_init;
	CComPtr<ID3D11ComputeShader> m_update, m_updateOnBeat;

	bool m_failed = false;
	bool m_pending = false;
	__m128i m_hash;

	void dropShaders();

public:

	StateShaders();

	~StateShaders();

	operator bool() const;

	// Post all 2 or 3 jobs, but only wait for the first one, the init shader
	HRESULT compile( const vector<EffectStateShader> &effects, UINT& totalStateSize );

	HRESULT bindInitShader();

	HRESULT bindUpdateShader( bool isbeat );

protected:

	void onRenderSizeChanged() override;
};