#pragma once

// EffectBase1 requires the effect structure to have an effect state. Not all effects use that so this class implements an empty one.
struct EmptyStateData
{
	EmptyStateData() = default;
	template<class A>
	EmptyStateData( const A& ) { }
	const StateShaderTemplate* shaderTemplate() { return nullptr; }
	UINT stateSize() { return 0; }
	HRESULT defines( Hlsl::Defines& def ) const { return S_FALSE; }
	bool operator==( const EmptyStateData& ) { return true; }
};