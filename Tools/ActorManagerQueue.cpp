#include "ActorManagerQueue.h"
#include <mutex>
#include "Profiler.h"

namespace Voxel {
	void ActorManagerQueue::Resolve() {
		SM_PROFILE_ZONE;
		std::lock_guard lk{ funcsQueueLock };
		while (!funcsQueue.empty())
		{
			std::function<void()> func = funcsQueue.front();
			funcsQueue.pop();
			func();
		}
	}
	
	void ActorManagerQueue::Clear()
	{
		std::lock_guard lk{ funcsQueueLock };
		funcsQueue = std::queue<std::function<void()>>();
	}
	
	void ActorManagerQueue::AddFunction(std::function<void()> func)
	{
		std::lock_guard lk{ funcsQueueLock };
		funcsQueue.push(func);
	}
}
