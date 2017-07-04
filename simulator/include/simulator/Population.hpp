/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Agent.hpp"
#include "model/AnimatWorld.hpp"
#include <thread>
#include <vector>

namespace simulator {
    class Population
    {
      public:
        Population(int const popSize, model::AnimatWorld & animatWorld);

        void update();

        /// update the agents based on some optimiation process
        void optimize(long const tick, 
                      int const everyN = 100,
                      bool const withMutations = true);

      private:

        /// The size of the population
        int const m_popSize;

        /// Models the simulated world
        model::AnimatWorld & m_animatWorld;

        /// The agents to be simulated
        std::vector<simulator::Agent> m_agents;

        /// The index of the elite agent
        int m_eliteIndex;

        /// Regenerate the population based on distances travelled
        /// with some offspring updated if withMutations is true
        void regenerate(bool const withMutations = true);
    };
}