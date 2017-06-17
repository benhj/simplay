/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Agent.hpp"
#include "model/AnimatWorld.hpp"
#include <thread>
#include <vector>

namespace simulator {
    class Simulation
    {
      public:
        Simulation(int const popSize);

        /// Initializes and starts the main simulation thread
        void start();

        /// Returns a reference to the simulated world
        model::AnimatWorld & animatWorld();
      private:
        /// The main simulation loop runs on this thread
        std::thread m_simThread;

        /// The size of the population
        int const m_popSize;

        /// Models the simulated world
        model::AnimatWorld m_animatWorld;

        /// The agents to be simulated
        std::vector<simulator::Agent> m_agents;

        /// The simulation loop that runs in thread
        void loop();
    };
}