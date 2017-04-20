// Copyright (c) 2017 Ben Jones

#pragma once

#include <thread>
#include <functional>

namespace graphics { namespace detail {

	class ThreadRunner
	{
      public:

      	void go(std::function<void()> const & func)
      	{
      		if (m_incThread.joinable()) {
      			m_incThread.join();
      		}
      		m_incThread = std::thread([=]{ func();});
      	}

      private:
      	std::thread m_incThread;
	};

}
}