#pragma once
#include "../Profiler.h"
#include <functional>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <future>
#include <string>
#include <sstream>
#include "../../vendor/josuttis_jthread/jthread.hpp"
#include "../../vendor/josuttis_jthread/condition_variable_any2.hpp"
#include "../../vendor/ofats_any_invocable/invocable.h"

namespace tk {
	template<typename T, typename ...Args>
	auto TupleAppend(T&& t, Args&&...args)
	{
		return std::tuple_cat(
			std::forward<T>(t),
			std::forward_as_tuple(args...)
		);
	}

	template<typename F, typename ...FrontArgs>
	decltype(auto) BindFront(F&& f, FrontArgs&&...frontArgs)
	{
		return [f = std::forward<F>(f),
			frontArgs = std::make_tuple(std::forward<FrontArgs>(frontArgs)...)]
			(auto&&...backArgs)
			{
				return std::apply(f, TupleAppend(frontArgs, std::forward<decltype(backArgs)>(backArgs)...));
			};
	}

	template <typename Lambda, typename ... Args>
	auto CaptureCall(Lambda&& lambda, Args&& ... args) {
		return [
			lambda = std::forward<Lambda>(lambda),
				capture_args = std::make_tuple(std::forward<Args>(args) ...)
		](auto&& ... original_args)mutable {
			return std::apply([&lambda](auto&& ... args) {
				lambda(std::forward<decltype(args)>(args) ...);
			}, std::tuple_cat(
				std::forward_as_tuple(original_args ...),
				std::apply([](auto&& ... args) {
					return std::forward_as_tuple< Args ... >(
						std::move(args) ...);
				}, std::move(capture_args))
			));
		};
	}

	class Task {
	public:
		Task() = default;
		Task(const Task&) = delete;
		Task(Task&& other) noexcept : executor{ std::move(other.executor) } {}
		Task& operator=(const Task&) = delete;
		Task& operator=(Task&& other) noexcept {
			if (&other != this)
			{
				executor = std::move(other.executor);
			}
			return *this;
		}

		void operator ()()
		{
			executor();
		}
		operator bool() const {
			return (bool)executor;
		}
		template<typename F, typename...A>
		static auto Make(F&& function, A&&...args)
		{
			std::promise<std::invoke_result_t<F, A...>> promise;
			auto future = promise.get_future();
			return std::make_pair(
				Task{ std::forward<F>(function), std::move(promise), std::forward<A>(args)... },
				std::move(future)
			);
		}
	private:
		template<typename F, typename P, typename...A>
		Task(F&& function, P&& promise, A&&...args)
		{
			executor = CaptureCall([
				function = std::forward<F>(function),
				promise = std::forward<P>(promise)
			](auto&& ... args) mutable {
				try {
					if constexpr (std::is_void_v<std::invoke_result_t<F, A...>>) {
						function(args...);
						promise.set_value();
					}
					else {
						promise.set_value(function(args...));
					}
				}
				catch (...) {
					promise.set_exception(std::current_exception());
				}
			}, std::forward<A>(args) ...);
		}
		ofats::any_invocable<void()> executor;
	};

	class ThreadPool
	{
	public:
		ThreadPool(size_t workersCount)
		{
			workers.reserve(workersCount);
			for (size_t i = 0; i < workersCount; i++)
				workers.emplace_back(this);
		}

		~ThreadPool()
		{
			for (auto& w : workers)
			{
				w.RequestStop();
			}
		}
		template<typename F, typename...A>
		auto Run(F&& function, A&&...args)
		{
			auto [task, future] = tk::Task::Make(std::forward<F>(function), std::forward<A>(args)...);
			{
				std::lock_guard lk{ taskQueueMutex };
				tasks.push_back(std::move(task));
			}
			taskQueueCV.notify_one();
			return std::move(future);
		}

		void WaitForAllDone()
		{
			std::unique_lock lk{ taskQueueMutex };
			allDoneCV.wait(lk, [this] { return tasks.empty(); });
		}
	private:
		Task GetTask(std::stop_token& st)
		{
			Task task;
			std::unique_lock lk{ taskQueueMutex };
			taskQueueCV.wait(lk, st, [this] { return !tasks.empty(); });
			if (!st.stop_requested()) {
				task = std::move(tasks.front());
				tasks.pop_front();

				if (tasks.empty())
					allDoneCV.notify_one();
			}
			return task;
		}

		class Worker
		{
		public:
			Worker(ThreadPool* pool)
				: thread(BindFront(&Worker::RunKernel, this)),
				pool(pool)
			{}

			void RequestStop()
			{
				thread.request_stop();
			}
		private:
			void RunKernel(std::stop_token st)
			{
				static size_t threadN = 0;
				SM_PROFILE_SET_THREAD_NAME(GetThreadName(threadN++).c_str());

				while (auto task = pool->GetTask(st))
				{
					task();
					task = {};
				}
			}
			std::string GetThreadName(size_t threadN) {
				std::stringstream ss;
				ss << "ThreadPool Kernel ";
				ss << threadN;
				return ss.str();
			}

			ThreadPool* pool;
			std::jthread thread;
		};
		std::mutex taskQueueMutex;
		std::condition_variable_any2 taskQueueCV;
		std::condition_variable allDoneCV;
		std::deque<Task> tasks;
		std::vector<Worker> workers;
	};
}
