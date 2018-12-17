#pragma once

// Minimally sufficient interface for the AVS code to render these effects with DX.
// AVS contains quite a bit of C++ code and doesn't use precompiled headers, having this minimal interface helps a lot with rebuild times.
class iRootEffect
{
public:
	virtual ~iRootEffect() { }

	// Only called on the root C_RenderListClass object
	virtual HRESULT renderRoot( char visdata[ 2 ][ 2 ][ 576 ], int isBeat ) { return CO_E_NOT_SUPPORTED; }
};