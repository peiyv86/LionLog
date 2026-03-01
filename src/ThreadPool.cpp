#include"ThreadPool.h"
#include<iostream>
#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<future>
#include<memory>

ThreadPool::ThreadPool(size_t N) :stop(false)
	{
		workers.reserve(N);
		for (int i = 0; i < N; i++)
		{
			workers.emplace_back([this]() {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> ulk(mtx);
						cv.wait(ulk, [this] {return !tasks.empty() || stop; });
						if (tasks.empty() && stop)return;
						task = tasks.front();
						tasks.pop();
					}
					try
					{
						task();
					}
					catch (...)
					{
						std::cout << "Something Error.\n";
					}
				}
				});
		}
	}

ThreadPool::~ThreadPool()
	{
		{
			std::lock_guard<std::mutex> lk(mtx);
			stop = true;
		}
		cv.notify_all();
		for (auto& w : workers)
		{
			w.join();
		}
		std::cout << "Threads Stopped.\n";
	}
