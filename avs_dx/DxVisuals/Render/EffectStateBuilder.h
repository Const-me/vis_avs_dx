#pragma once
#include "../Hlsl/FunctionBuilder.h"
#include "../Hlsl/ShaderBuilder.h"

class EffectStateBuilder
{
public:
	using fnBuildShaderFunc = std::function<HRESULT( int stateOffset, Hlsl::FunctionBuilder& hlsl )>;

	// Declare an effect state block that will be updated on every frame.
	HRESULT state( int stateSize, fnBuildShaderFunc &&fnUpdate );
	// Declare an effect state block that will be updated on every frame, plus some extra update code when the frame has a beat detected.
	HRESULT state( int stateSize, fnBuildShaderFunc &&fnUpdate, fnBuildShaderFunc &&fnUpdateOnBeat, bool alwaysRunUpdate = true );

	// Some effects want to consume extra variables updated by CPU, we pass that data in constant buffers. The buffer will have D3D11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE. Disabled for now.
	// HRESULT constantBuffer( size_t cb, const char* hlslName, Resource& buffer );


private:
	fnBuildShaderFunc m_stateUpdate, m_stateBeat;
	bool m_alwaysUpdateState = true;
};