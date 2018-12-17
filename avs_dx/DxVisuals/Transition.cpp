#include "stdafx.h"
#include "Transition.h"
#include "Resources/staticResources.h"

void createTransition( std::unique_ptr<iTransition>& up )
{
	up = std::make_unique<Transition>();
}

Transition::~Transition()
{ }

HRESULT Transition::prepare( char visdata[ 2 ][ 2 ][ 576 ], int isBeat )
{
	// Upload visualization data to GPU
	CHECK( StaticResources::sourceData.update( visdata, isBeat ) );

	context->OMSetBlendState( StaticResources::blendPremultipliedAlpha, nullptr, 0xffffffff );

	// Handle resize
	const CSize currentSize = getRenderSize();
	if( m_renderSize != currentSize )
	{
		m_targets1.destroy();
		m_targets2.destroy();
		m_renderSize = currentSize;
	}

	return S_OK;
}

HRESULT Transition::renderSingle( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e )
{
	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );
	}

	CHECK( e.renderRoot( 0 != isBeat, m_targets1 ) );

	CHECK( present( m_targets1.lastWritten() ) );

	return S_OK;
}

HRESULT Transition::renderTransition( char visdata[ 2 ][ 2 ][ 576 ], int isBeat, iRootEffect &e1, iRootEffect &e2, int trans, float sintrans ) 
{
	{
		CSLock __lock( renderLock );
		CHECK( prepare( visdata, isBeat ) );
	}

	const bool beat = 0 != isBeat;

	CHECK( e1.renderRoot( beat, m_targets1 ) );
	CHECK( e2.renderRoot( beat, m_targets2 ) );

	// TODO: implement the transition
	return E_NOTIMPL;
}