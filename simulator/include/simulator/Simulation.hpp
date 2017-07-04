/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Agent.hpp"
#include "Population.hpp"
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

        /// Models the simulated world
        model::AnimatWorld m_animatWorld;

        /// Tracks the population of agents
        Population m_population;

        /// The simulation loop that runs in thread
        void loop();

        /// Called by doLoop and when stabilizing the physics model
        void doLoop(long const tick, 
                    int const everyN = 100, 
                    bool const withMutations = true);
    };
}