// Copyright (c) 2017 Ben Jones

#pragma once

#include <OpenGL/gl.h>
#include <thread>
#include <chrono>

namespace graphics { namespace detail {

	template <typename ValueType>
	class ThreadedVarIncrementer
	{
      public:
      	ThreadedVarIncrementer(ValueType & value, 
      					       ValueType const incAmount, 
      					       int const iterations,
      					       long const sleepBetween)
      	: m_value(value)
      	, m_incAmount(incAmount)
      	, m_iterations(iterations)
      	, m_sleepBetween(sleepBetween)
      	{
      	}

      	void go()
      	{
      		if (m_incThread.joinable()) {
      			m_incThread.join();
      		}
      		m_incThread = std::thread([&]{
      			for(auto i = 0 ; i < m_iterations; ++i) {
      				m_value += incAmount;
      				using namespace std::chrono_literals;
      				std::this_thread::sleep_for((m_sleepBetween)ms);
      			}
      		});
      	}

      private:
      	ValueType & m_value;
      	ValueType const m_incAmount;
      	long const m_sleepBetween;
      	int const m_iterations;
      	std::thread m_incThread;
	};

}

}