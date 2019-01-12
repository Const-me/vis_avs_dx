#pragma once

namespace Hlsl
{
	// Utility class that wraps Vista+ thread pool API into something more C++ friendly
	class Worker
	{
		PTP_WORK m_work = nullptr;

		static void __stdcall callbackStatic( TP_CALLBACK_INSTANCE *Instance, void* pv, PTP_WORK Work );

	public:

		Worker();
		Worker( const Worker& ) = delete;
		Worker( Worker&& ) = delete;
		~Worker();

		// Cancel jobs that weren't yet started, block caller thread wait for any pending callbacks to complete, and shutdown the async worker
		void shutdown();

		void launch();

		// Block caller thread and wait until all pending callbacks complete, if any
		void join();

	protected:

		virtual void workCallback() = 0;
	};
}