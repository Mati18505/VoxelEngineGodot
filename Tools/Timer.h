#pragma once
#include <chrono>
#include <functional>

namespace Voxel {
	class Timer {
		std::chrono::steady_clock::time_point start;
		std::function<void(double ms)> fnc;
	public:
		Timer(std::function<void(double ms)> fnc)
			: fnc(fnc)
		{
			start = std::chrono::high_resolution_clock::now();
		}
	
		~Timer()
		{
			auto stop = std::chrono::high_resolution_clock::now();
			double miliseconds = std::chrono::duration<double, std::milli>(stop - start).count();
			fnc(miliseconds);
		}
	};
}
