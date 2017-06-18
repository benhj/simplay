/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>
#include <algorithm>

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
        long tick = 0;
        while(true) {
            
            for (int p = 0; p < m_popSize; ++p) {
                auto & agent = m_agents[p];

                // if physics broke, reinit position in world
                if(agent.update() == -1) {
                    m_animatWorld.randomizePositionSingleAnimat(p, 10, 10);
                }
            }
            doOptimizations(tick);
            usleep(500);
            ++tick;
        }
    }

    void Simulation::regeneratePopulation()
    {
        // store all fitness values
        std::vector<std::pair<int, double> > fitnesses;
        fitnesses.reserve(m_popSize);
        auto i = 0;
        for (auto const & agent : m_agents) {
            auto dm = agent.distanceMoved();
            if(dm>50)dm=-1;
            fitnesses.emplace_back(i, dm);
            ++i;
        }

        // sort according to fitness
        std::sort(std::begin(fitnesses), std::end(fitnesses),
                  [](std::pair<int, double> const & a,
                     std::pair<int, double> const & b) {
                      return b.second < a.second;
                  });

        for(auto const & f : fitnesses) {
            std::cout<<f.second<<std::endl;
        }

        // pick the top 10 and regen rest of population out of those
        i = 0;
        for (auto & agent : m_agents) {

            // inherit neat genome from fit agent
            agent.inheritNeat(m_agents[fitnesses[i].first]);

            // mutate inherited genome
            agent.modifyController(); 
            ++i;
            if (i == 10) {
                i = 0;
            }
        }

    }

    void Simulation::doOptimizations(long const tick)
    {
        /// How often to 'mutate the controllers'
        if(tick % 100 == 0 && tick > 0) {

            // record distances travelled for each agent
            for (auto & agent : m_agents) {
                agent.recordDistanceMoved();
            }

            regeneratePopulation();

            m_animatWorld.randomizePositions(10, 10);

            // set agent starting positions
            for (auto & agent : m_agents) {
                agent.recordStartPosition();
            }
        }
    }

}