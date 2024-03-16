#include "ActorManagerQueue.h"
#include <mutex>

namespace Voxel {
	void ActorManagerQueue::Resolve() {
		funcsQueueLock.lock();
		while (!funcsQueue.empty())
		{
			std::function<void()> func = funcsQueue.front();
			funcsQueue.pop();
			func();
		}
		funcsQueueLock.unlock();
	}
	
	void ActorManagerQueue::Clear()
	{
		funcsQueueLock.lock();
		funcsQueue = std::queue<std::function<void()>>();
		funcsQueueLock.unlock();
	}
	
	void ActorManagerQueue::AddFunction(std::function<void()> func)
	{
		funcsQueueLock.lock();
		funcsQueue.push(func);
		funcsQueueLock.unlock();
	}
}
