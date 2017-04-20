// Copyright (c) 2017 Ben Jones

#pragma once

#include <atomic>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <iostream>

namespace graphics { namespace detail {

	class ThreadRunner
	{
      public:
	    ThreadRunner() { go(); }

      	void add(std::function<void()> const & func)
      	{
      		std::unique_lock<std::mutex> ul(m_mutex);
			m_queue.push(func);
			ul.unlock();
			m_cond.notify_one();
      	}

      private:
	  	std::queue<std::function<void()>> m_queue;
		std::mutex m_mutex;
		std::condition_variable m_cond;
      	std::thread m_incThread;
	    std::atomic<bool> m_shutdown;

	    void go()
		{
			m_incThread = std::thread([&]{ 
				while (!m_shutdown) {
					std::unique_lock<std::mutex> ul(m_mutex);
					while (m_queue.empty()) { m_cond.wait(ul); }
					m_queue.front()();
					m_queue.pop();
				}
			});
		}
	};
}
}