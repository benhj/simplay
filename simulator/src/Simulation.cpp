/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>
#include <algorithm>

namespace simulator {

    Simulation::Simulation(int const popSize)
    : m_animatWorld(popSize)
    , m_population(popSize, m_animatWorld)
    {
        m_animatWorld.randomizePositions(10, 10);
    }

    model::AnimatWorld & Simulation::animatWorld()
    {
        return m_animatWorld;
    }

    void Simulation::start()
    {
        // run simulation proper
        m_simThread = std::thread(&Simulation::loop, this);
    }

    void Simulation::doLoop(long const tick, 
                            int const everyN, 
                            bool const withMutations)
    {
        m_population.update();
        m_population.optimize(tick, everyN, withMutations);
    }

    void Simulation::loop()
    {
        long tick = 0;
        while(true) {
            doLoop(tick, 500);
            //usleep(500);
            ++tick;
        }
    }
}
