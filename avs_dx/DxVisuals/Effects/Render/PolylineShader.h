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

HRESULT updateLineShaders( Shader<PolylineShaderGS>& gs, Shader<PolylineShaderPS>& ps );