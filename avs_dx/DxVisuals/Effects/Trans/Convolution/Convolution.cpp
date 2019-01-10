#include "stdafx.h"
#include "Convolution.h"
#include "convolutionHacks.h"

IMPLEMENT_APE_EFFECT( Convolution, "Holden03: Convolution Filter", "convolution.ape" );

ConvolutionStructs::CsData::CsData( const C_RBASE& nativeFx )
{
	size = getRenderSize();

	const KernelData kd = getKernelData( &nativeFx );
	wrap = kd.wrap;
	kernel = kd.values;
}

void ConvolutionStructs::CsData::dispatch() const
{
	const uint32_t writesPerGroup = threadsPerGroup - 3 - 3;
	context->Dispatch( ( size.cx + writesPerGroup - 1 ) / writesPerGroup, ( size.cy + writesPerGroup - 1 ) / writesPerGroup, 1 );
}

Convolution::Convolution( C_RBASE* pThis ):
	EffectBase1( pThis )
{ }

HRESULT Convolution::render( bool isBeat, RenderTargets& rt )
{
	const MiscData misc = getMiscData( avs );
	if( !misc.enabled )
		return S_FALSE;

	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;

	const int passes = misc.twoPasses ? 2 : 1;
	for( int i = 0; i < passes; i++ )
	{
		BoundSrv<eStage::Compute> boundRead;
		BoundUav boundWrite;
		const auto& cs = renderer.compute();
		CHECK( rt.computeToNext( cs.bindInput, boundRead, cs.bindOutput, boundWrite ) );
		cs.dispatch();
	}
	return S_OK;
}