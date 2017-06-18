/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>

namespace simulator {
    Simulation::Simulation(int const popSize)
    : m_popSize(popSize)
    , m_animatWorld(popSize)
    {
        m_animatWorld.randomizePositions(10, 10);
        m_agents.reserve(popSize);
        for(int i = 0;i<popSize;++i){
            m_agents.emplace_back(m_animatWorld.animat(i));
        }
    }

    model::AnimatWorld & Simulation::animatWorld()
    {
        return m_animatWorld;
    }

    void Simulation::start()
    {
        m_simThread = std::thread(&Simulation::loop, this);
    }

    void Simulation::loop()
    {
        for(int tick = 0; tick < 10000; ++tick) {
            
            for (int p = 0; p < m_popSize; ++p) {
                auto & agent = m_agents[p];

                // if physics broke, reinit position in world
                if(agent.update() == -1) {
                    m_animatWorld.randomizePositionSingleAnimat(p, 10, 10);
                }
            }
            /// How often to 'mutate the controllers'
            if(tick % 250 == 0 && tick > 0) {
                m_animatWorld.randomizePositions(10, 10);
            }
            usleep(500);
        }
    }

}