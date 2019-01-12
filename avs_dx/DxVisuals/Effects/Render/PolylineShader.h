#pragma once
#include "../shadersCode.h"
#include <Render/Shader.hpp>

struct PolylineShaderGS: public Hlsl::Render::PolylineGS
{
	template<class S>
	HRESULT updateAvs( const S& s )
	{
		return updateLineThickness();
	}

private:

	HRESULT updateLineThickness();
};

struct PolylineShaderPS : public Hlsl::Render::PolylinePS
{
	template<class S>
	HRESULT updateAvs( const S& s )
	{
		return updateLineThickness();
	}

private:

	HRESULT updateLineThickness();
};

// This is for superscope effect. Because of the switchable line/dots modes, normal update workflow doesn't update and therefore doesn't recompile these shaders.
HRESULT updateLineShaders( Shader<PolylineShaderGS>& geometry, Shader<PolylineShaderPS>& pixel );