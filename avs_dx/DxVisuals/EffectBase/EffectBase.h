#pragma once
class EffectStateBuilder;
class RenderTarget;

class EffectBase
{
public:
	virtual HRESULT renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat ) { return CO_E_NOT_SUPPORTED; }

	virtual HRESULT stateDeclarations( EffectStateBuilder &builder ) = 0;
};