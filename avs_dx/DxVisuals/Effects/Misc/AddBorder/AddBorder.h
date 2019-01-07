#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Effects/Common/ApeEffectBase.h>

using namespace Hlsl::Misc::AddBorder;

struct AddBorderStructs
{
	using AvsState = C_RBASE;
	using VsData = AddBorderVS;
	using PsData = AddBorderPS;
	using StateData = EmptyStateData;
};

class AddBorder: public EffectBase1<AddBorderStructs, ApeEffectBase>
{
	CComPtr<ID3D11Buffer> m_ib;
	CComPtr<ID3D11Buffer> m_cb;

	struct SourceData
	{
		int size, color;
		Vector4 getConstantBuffer() const;
	};

	SourceData queryData() const;

	HRESULT ensureIndexBuffer();

public:

	AddBorder( C_RBASE* pThis );

	DECLARE_EFFECT()

	void bindResources() override;

	HRESULT render( bool isBeat, RenderTargets& rt ) override;
};