#pragma once
#include <mutex>
#include <queue>
#include <functional>

namespace Voxel {
	class ActorManagerQueue {
	public:
		ActorManagerQueue() {}
	
		void AddFunction(std::function<void()> func);
		void Resolve();
		void Clear();
	
	private:
		std::mutex funcsQueueLock;
		std::queue<std::function<void()>> funcsQueue;
	};
}
