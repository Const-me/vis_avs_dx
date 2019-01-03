#pragma once
#include <memory>
#include "iRootEffect.h"

// This interface acts as a rendering entry point of the whole DxVisuals library.
class iTransition
{
public:
	virtual ~iTransition() { }

	virtual HRESULT renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e ) = 0;

	virtual HRESULT renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e1, iRootEffect &e2, int trans, float sintrans ) = 0;
};

bool createTransitionInstance();

iTransition* getTransition();