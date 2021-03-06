/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>
#include <algorithm>
#include <thread>

namespace simulator {

    Simulation::Simulation(int const popSize)
    : m_animatWorld(popSize)
    , m_population(popSize, m_animatWorld)
    , m_paused(false)
    , m_sleepDuration{0}
    {
        //m_animatWorld.randomizePositions(10, 10);
    }

    void Simulation::activateEvolution()
    {
        m_population.activateEvolution();
    }
    void Simulation::deactivateEvolution()
    {
        m_population.deactivateEvolution();
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

    void Simulation::pause()
    {
        m_paused = true;
    }

    void Simulation::resume()
    {
        m_paused = false;
    }

    void Simulation::setSleepDuration(int const duration)
    {
        m_sleepDuration = duration;
    }

    void Simulation::doLoop(long const tick, 
                            int const everyN, 
                            bool const withMutations)
    {
        m_population.update(tick, everyN, withMutations);
    }

    void Simulation::loop()
    {
        long tick = 0;
        while(true) {

            // TODO: change from a spin-lock-esque
            // pattern to a proper condition var.
            // I'm being lazy. Need a holiday.
            if(!m_paused) {
                doLoop(tick, 500);
                usleep(m_sleepDuration);
                ++tick;
            }
        }
    }

    void Simulation::enableCollisionHandling()
    {
        auto & agents = m_population.getAgents();
        for(auto & agent : agents) {
            agent.enableCollisionHandling();
        }
    }
    void Simulation::disableCollisionHandling()
    {
        auto & agents = m_population.getAgents();
        for(auto & agent : agents) {
            agent.disableCollisionHandling();
        }

    }
}
