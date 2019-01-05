#pragma once
class C_RBASE;

// This interface acts as a rendering entry point of the whole DxVisuals library.
class iTransition
{
public:
	virtual ~iTransition() { }

	virtual HRESULT renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *pRBase ) = 0;

	virtual HRESULT renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, const C_RBASE *e1, const C_RBASE *e2, int trans, float sintrans ) = 0;
};

bool createTransitionInstance();

iTransition* getTransition();