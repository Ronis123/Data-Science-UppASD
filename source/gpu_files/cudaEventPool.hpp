/*
 * Cuda event pool class
 *  Niklas Fejes 2013
 */
#ifndef __CUDA_EVENT_POOL_HPP__
#define __CUDA_EVENT_POOL_HPP__

#include <vector>
#include <cuda.h>

class CudaEventPool {
public:
	// Event wrapper class
	class Event {
		friend CudaEventPool;
		bool active;
		cudaEvent_t _event;
		static void deactivate_callback(cudaStream_t, cudaError_t, void * e);
	public:
		Event();
		cudaEvent_t event();
		void deactivate();
		void addDeactivateCallback(cudaStream_t stream);
	};
private:
	std::vector<Event*> stack;

public:
	// Get an activated event from the pool
	Event &get();

	// Destructor
	~CudaEventPool();
};

#endif

