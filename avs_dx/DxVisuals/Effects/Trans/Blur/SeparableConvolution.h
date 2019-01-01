#pragma once
#include <Utils/resizeHandler.h>
#include <Render/Shader.hpp>
#include <Effects/shadersCode.h>
#include "KernelValues.h"
#include <Resources/RenderTargets.h>

// This class runs a separable convolution filter on the render target image.
// Because it's separable, we split the job in two passes, first along X, then along Y. Compared to naive implementation, this way it's much faster.
class SeparableConvolution: public ResizeHandler
{
	// Transposed texture for the intermediate buffer
	CComPtr<ID3D11UnorderedAccessView> m_uav;
	CComPtr<ID3D11ShaderResourceView> m_srv;

	void onRenderSizeChanged() override
	{
		destroy();
	}

	struct ShaderParams
	{
		CSize inputSize;
		const ConvolutionKernel* pKernel;
	};

	struct PassShaderData : public Hlsl::Trans::Blur::LinearConvolutionCS
	{
		HRESULT updateAvs( const ShaderParams& params );

		HRESULT defines( Hlsl::Defines& def ) const;

		void dispatch() const;

	private:
		const ConvolutionKernel* m_pKernel;
		CSize m_groups;
	};

	using PassShader = Shader<PassShaderData>;
	std::array<PassShader, 2> m_shaders;
	static HRESULT updateShader( PassShader& s, Binder& binder, const ShaderParams& params );

	void destroy();

	HRESULT ensureBuffer();
	
public:

	// Update bindings, if necessary, compile the two compute shaders.
	HRESULT update( Binder& binder, const ConvolutionKernel *pKernel );

	// Run the convolution filter
	HRESULT run( RenderTargets& rt );
};