#pragma once

#include<iostream>
#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<future>
#include<memory>

class ThreadPool
{
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex mtx;
	std::condition_variable cv;
	bool stop;
public:
	ThreadPool(size_t N);

	template<typename F, typename...Args>
	auto enqueue(F&& f, Args&&... arg)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		using r_type = std::invoke_result_t<F, Args...>;
		auto p_task = std::make_shared<std::packaged_task<r_type()>>
			(std::bind(std::forward<F>(f), std::forward<Args>(arg)...));
		std::future<r_type> out = p_task->get_future();
		{
			std::unique_lock<std::mutex> ulk(mtx);
			tasks.emplace([p_task] {(*p_task)(); });
		}
		cv.notify_one();
		return out;
	}
	~ThreadPool();
};