#include "stdafx.h"
#include "CompileWorker.h"
#include "../compile.h"
#include <Resources/createShader.hpp>

using namespace Hlsl;

CompilerBase::CompilerBase( eStage stage, const CAtlMap<CStringA, CStringA>& inc, Job* pPending, uint8_t capacity ) :
	m_stage( stage ), m_includes( inc ), m_pending( pPending ), m_capacity( capacity )
{ }

CompilerBase::~CompilerBase()
{
	// This is required due to destruction order.
	// Otherwise, the runtime will destroy this class (including e.g. the critical section), only then call base class destructor which will shut down the background jobs, very rare crash accessing destroyed critical section.
	// Pure virtual call crash also possible, invoking the workCallback() method.
	Worker::shutdown();
}

void CompilerBase::submit( const CStringA& hlsl, const Defines& def, const Job* first, uint8_t count )
{
	assert( count <= m_capacity );

	{
		CSLock __lock( m_cs );
		m_version++;
		m_pendingLaunch = m_pendingCompletion = count;
		if( count <= 0 )
		{
			m_status = eAsyncStatus::Completed;
			return;
		}

		eastl::copy_n( first, count, m_pending );
		m_hlsl = hlsl;
		m_defines = def;
		m_result = false;
		m_status = eAsyncStatus::Pending;
	}

	for( int i = 0; i < count; i++ )
		Worker::launch();
}

void CompilerBase::cancelPending()
{
	CSLock __lock( m_cs );
	m_version++;
}

struct CompilerBase::ThreadContext
{
	Hlsl::Defines defines;
	CStringA hlsl;
	vector<uint8_t> dxbc;
	Job job;
	uint32_t version;

	void initialize( const CompilerBase& compiler, const Job& j )
	{
		defines = compiler.m_defines;
		hlsl = compiler.m_hlsl;
		dxbc.clear();
		job = j;
		version = compiler.m_version;
	}
};

namespace
{
	using pfnCreateShader = HRESULT( *)( const vector<uint8_t> &dxbc, CComPtr<IUnknown>& res );

	template<eStage s>
	HRESULT createShaderUnk( const vector<uint8_t> &dxbc, CComPtr<IUnknown>& unknown )
	{
		ShaderPtr<s> shader;
		const HRESULT hr = createShader( dxbc, shader );
		unknown.Attach( shader.Detach() );
		return hr;
	}

	static const array<pfnCreateShader, 4> s_createShaders =
	{
		&createShaderUnk<eStage::Compute>,
		&createShaderUnk<eStage::Vertex>,
		&createShaderUnk<eStage::Geometry>,
		&createShaderUnk<eStage::Pixel>,
	};
}

void CompilerBase::workCallback()
{
	thread_local ThreadContext context;

	{
		CSLock __lock( m_cs );
		if( 0 == m_pendingLaunch )
			return;
		const uint8_t index = m_pendingLaunch - 1;
		m_pendingLaunch--;
		context.initialize( *this, m_pending[ index ] );
	}

	const HRESULT hr = compileJob( context );

	if( FAILED( hr ) )
	{
		CSLock __lock( m_cs );
		if( m_version != context.version )
			return;

		for( IUnknown** pp : context.job.results )
		{
			if( nullptr == pp || nullptr == *pp )
				continue;
			( *pp )->Release();
			( *pp ) = nullptr;
		}

		assert( m_pendingCompletion > 0 );
		m_pendingCompletion--;
		m_result.combine( hr );
		if( 0 == m_pendingCompletion )
			m_status = eAsyncStatus::Failed;
		return;
	}
}

HRESULT CompilerBase::compileJob( ThreadContext& context )
{
	// Prepare the defines
	if( nullptr != context.job.fnExtraDefines )
		context.job.fnExtraDefines( context.defines );

	// Run the compiler
	SILENT_CHECK( Hlsl::compile( m_stage, context.hlsl, context.job.name, m_includes, context.defines, context.dxbc ) );

	// Create the shader, apparently the device is thread safe: https://docs.microsoft.com/en-us/windows/desktop/direct3d11/overviews-direct3d-11-render-multi-thread-intro
	CComPtr<IUnknown> result;
	SILENT_CHECK( s_createShaders[ (uint8_t)m_stage ]( context.dxbc, result ) );

	// If user has asked for the callback, invoke it.
	if( context.job.fnCompiledShader )
		context.job.fnCompiledShader( context.job.compiledShaderContext, context.dxbc );

	CSLock __lock( m_cs );

	if( m_version != context.version )
		return S_FALSE;	// New shader version already requested, ignore this result

	// Replace shaders in the destination
	for( IUnknown** pp : context.job.results )
	{
		if( nullptr == pp )
			continue;
		if( nullptr != *pp )
			( *pp )->Release();
		( *pp ) = result;
		( *pp )->AddRef();
	}

	assert( m_pendingCompletion > 0 );
	m_pendingCompletion--;
	m_result.combine( true );
	if( 0 == m_pendingCompletion )
		m_status = m_result.succeeded() ? eAsyncStatus::Completed : eAsyncStatus::Failed;

	return S_OK;
}