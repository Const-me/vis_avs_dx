#pragma once
#include "Stage.h"
#include <Hlsl/Defines.h>
#include <Utils/resizeHandler.h>

// Non-template base class to save a couple kilobytes of code size.
class ShaderBase2 : public iResizeHandler
{
protected:

	enum struct eShaderState : uint8_t
	{
		Constructed,
		Updated,
		Failed,
		Good,
	};

	eShaderState m_state = eShaderState::Constructed;

	// If needed, set AVS_RENDER_SIZE and AVS_PIXEL_PARTICLES defines, and subscribe/unsubscribe for resize event accordingly.
	void setSizeDefines( const CStringA& hlsl, Hlsl::Defines &def );

public:

	~ShaderBase2();

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
};

// Holds a set of 2 shaders compiled from the same HLSL source code, the second one is used when beat is detected.
template<eStage stage>
class ShaderBase: public ShaderBase2
{
	ShaderPtr<stage> shader, beatShader;

public:

	static constexpr eStage shaderStage = stage;

	// Bind the shader. Returns false and does nothing if it's empty.
	bool bind( bool isBeat ) const;

	bool hasShader() const;

	void dropShader();

	IShader<stage> *ptr( bool isBeat ) const;

protected:

	HRESULT compile( const char* name, const CStringA& hlsl, const CAtlMap<CStringA, CStringA>& inc, Hlsl::Defines &def, bool usesBeat, std::vector<uint8_t>& dxbc );

	void onRenderSizeChanged() override;
};