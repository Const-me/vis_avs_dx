#pragma once

namespace Hlsl
{
	// Utility class that wraps Vista+ thread pool API into something more C++
	class Worker
	{
		PTP_WORK m_work = nullptr;

		static void __stdcall callbackStatic( PTP_CALLBACK_INSTANCE callbackInstance, void* pv, PTP_WORK Work );

	public:

		Worker();
		Worker( const Worker& ) = delete;
		Worker( Worker&& ) = delete;
		~Worker();

		// Cancel jobs that weren't yet started, block caller thread waiting for any in-flight callbacks to complete, and shutdown the worker
		void shutdownWorker();

		// Start the background work. 
		// You can post a work object one or more times (up to MAXULONG) without waiting for prior callbacks to complete. The callbacks will execute in parallel. To improve efficiency, the thread pool may throttle the threads.
		void launch();

		// Block caller thread and wait until all in-flight callbacks (if any) fail or complete
		void join();

	protected:

		// This abstract method will be called on the thread pool's threads, possibly multiple times in parallel.
		virtual void workCallback() = 0;
	};
}