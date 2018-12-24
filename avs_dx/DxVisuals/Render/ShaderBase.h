#pragma once
#include "Stage.h"
#include <Hlsl/Defines.h>
#include <Utils/resizeHandler.h>

template<eStage stage>
class ShaderBase: public iResizeHandler
{
	enum struct eShaderState : uint8_t
	{
		Constructed,
		Updated,
		Failed,
		Good,
	};

	eShaderState m_state = eShaderState::Constructed;

	ShaderPtr<stage> shader, beatShader;

public:

	static constexpr eStage shaderStage = stage;

	bool bind( bool isBeat ) const;

	bool hasShader() const;

	void dropShader();

	IShader<stage> *ptr( bool isBeat ) const;

protected:

	void setUpdated()
	{
		m_state = eShaderState::Updated;
	}

	void setFailed()
	{
		m_state = eShaderState::Failed;
	}
	bool isFailed() const
	{
		return m_state == eShaderState::Failed;
	}
	void setGood()
	{
		m_state = eShaderState::Good;
	}

	HRESULT compile( const char* name, const CStringA& hlsl, const CAtlMap<CStringA, CStringA>& inc, Hlsl::Defines &def, bool usesBeat, std::vector<uint8_t>& dxbc );

	void onRenderSizeChanged() override;
};