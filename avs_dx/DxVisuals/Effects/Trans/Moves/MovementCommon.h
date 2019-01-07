#pragma once
#pragma once
#include <Effects/Common/EffectImpl.hpp>
#include <Expressions/CompiledShader.h>
#include "GridMesh.h"
#include "Sampler.h"
using namespace Hlsl::Trans::Moves;

struct MovementStructs
{
	struct CommonStateData : public Expressions::Compiler
	{
		CommonStateData( const char *name, const Expressions::Prototype& proto ) :
			Compiler( name, proto, 0 ) 
		{ }

	protected:
		CSize screenSize;
		HRESULT updateScreenSize();
	};

	struct CommonVsData : public Expressions::CompiledShader<DMoveVS>
	{
		static HRESULT compiledShader( const vector<uint8_t>& dxbc );

	protected:
		HRESULT updateAvs( bool rectCoords );
	};
};

class MovementFx
{
	GridMesh m_mesh;
	Sampler m_sampler;

protected:

	HRESULT render( RenderTargets& rt, bool bilinear, bool wrap, UINT samplerSlot, bool blend, bool rectCoords );
};