#pragma once
#include <d3d11_1.h>

class BlendModes
{
	enum struct eCustom : uint8_t
	{
		Maximum,
		Sub1,
		Sub2,
		Multiply,
		Minimum
	};
	struct sState
	{
		CComPtr<ID3D11BlendState> bs;
		bool failed = false;
	};
	array<sState, 5> m_customModex;
	CComPtr<ID3D11BlendState1> m_xorMode;

	bool setCustom( eCustom mode );
	bool setXorBlend();

public:

	void destroy();

	bool setupBlending();

	static float getLineWidth();
};