#include "stdafx.h"
#include "Renderer.h"

void Renderer::updateState( bool beat )
{
	context->CSSetShader( beat ? m_updateStateBeat : m_updateState, nullptr, 0 );
	csSetUav( m_state.uav(), 0 );
	context->Dispatch( 1, 1, 1 );
}