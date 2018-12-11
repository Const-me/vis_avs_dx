#include "stdafx.h"
#include "Simple.h"

IMPLEMENT_EFFECT( Simple, C_SimpleClass )

HRESULT Simple::stateDeclarations( EffectStateBuilder &builder )
{
	constexpr auto c = dots.shaderKinds;
	static_assert( c[ 0 ] == eShaderKind::None );
	static_assert( c[ 1 ] == eShaderKind::Dynamic );
	static_assert( c[ 2 ] == eShaderKind::Dynamic );
	static_assert( c[ 3 ] == eShaderKind::Static );

	return E_NOTIMPL;
}