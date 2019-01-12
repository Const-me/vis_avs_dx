#pragma once
#include "Worker.h"
#include "../Defines.h"
#include <Render/Stage.h>

namespace Hlsl
{
	enum struct eAsyncStatus
	{
		Empty,
		Pending,
		Completed,
		Failed,
	};

	using pfnSetExtraDefines = void( *)( Hlsl::Defines& defines );

	using pfnCompiledShader = void( *)( void* pContext, const vector<uint8_t>& dxbc );

	struct Job
	{
		const char* name = nullptr;

		pfnSetExtraDefines fnExtraDefines = nullptr;

		// When finished, the compiler will replace both pointers (if non-null) with the newly compiler shader
		array<IUnknown**, 2> results = {};

		// Optional callback to invoke after the shader is compiled, receives DXBC bytecode. Used for creation of input layouts.
		pfnCompiledShader fnCompiledShader = nullptr;
		void* compiledShaderContext = nullptr;

		Job() = default;
		Job( const Job& ) = default;

		template<class I>
		Job( const char* n, I** pp, pfnSetExtraDefines def )
		{
			name = n;
			results[ 0 ] = (IUnknown**)pp;
			results[ 1 ] = nullptr;
			fnExtraDefines = def;
		}

		template<class I>
		Job( const char* n, I** pp1, I** pp2, pfnSetExtraDefines def = nullptr )
		{
			name = n;
			results[ 0 ] = (IUnknown**)pp1;
			results[ 1 ] = (IUnknown**)pp2;
			fnExtraDefines = def;
		}
	};

	// Non-template base class to help with compilation time and binary size
	class CompilerBase : public Worker
	{
		const eStage m_stage;
		const CAtlMap<CStringA, CStringA>& m_includes;

	public:
		CompilerBase( eStage stage, const CAtlMap<CStringA, CStringA>& inc, Job* pPending, uint8_t capacity );
		CompilerBase( const Worker& ) = delete;
		CompilerBase( Worker&& ) = delete;

		~CompilerBase();

		// Guards pending jobs also the resulting shader. Lock while accessing the pointer where the async compiler puts these shaders.
		CComAutoCriticalSection& compilerLock() const { return m_cs; }

		// Submit a new shader compilation job
		void submit( const CStringA& hlsl, const Hlsl::Defines& def, const Job* first, uint8_t count );

		// Cancel any in-flight requests, if any. It doesn't currently cancel anything just bumps version so the pending results are discarded.
		void cancelPending();

		eAsyncStatus asyncStatus() const;

	private:

		mutable CComAutoCriticalSection m_cs;
		CStringA m_hlsl;
		Hlsl::Defines m_defines;
		uint32_t m_version = 0;
		const uint8_t m_capacity;
		uint8_t m_pendingLaunch = 0, m_pendingCompletion = 0;
		eAsyncStatus m_status = eAsyncStatus::Empty;
		BoolHr m_result;
		Job* const m_pending;

		struct ThreadContext;

		void workCallback() override;

		HRESULT compileJob( ThreadContext& context );
	};

	template<uint8_t count>
	class CompileWorker : public CompilerBase
	{
		array<Job, count> m_pendingJobs = {};

	public:

		CompileWorker( eStage stage, const CAtlMap<CStringA, CStringA>& inc ) :
			CompilerBase( stage, inc, m_pendingJobs.data(), count ) { }
	};
}