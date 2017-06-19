/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>
#include <algorithm>

namespace simulator {
    Simulation::Simulation(int const popSize)
    : m_popSize(popSize)
    , m_animatWorld(popSize)
    , m_eliteIndex(0)
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
        // stabilise
        // std::cout<<"Stabilizing.."<<std::endl;
        // long tick = 0;
        // while(tick < 3000) {
        //     doLoop(tick, 100, false);
        //     ++tick;
        // }
        // std::cout<<"\nStabilized.."<<std::endl;

        // m_animatWorld.randomizePositions(10, 10);
        // for (auto & agent : m_agents) {
        //     agent.recordStartPosition();
        // }

        // run simulation proper
        m_simThread = std::thread(&Simulation::loop, this);
    }

    void Simulation::doLoop(long const tick, 
                            int const everyN, 
                            bool const withMutations)
    {
        for (int p = 0; p < m_popSize; ++p) {
            auto & agent = m_agents[p];

            // if physics broke, reinit position in world
            if(agent.update() == -1) {
                m_animatWorld.randomizePositionSingleAnimat(p, 10, 10);
                agent.recordStartPosition();
                agent.resetController();

                // give animat a fitter genome, the elite one in fact
                agent.inheritNeat(m_agents[m_eliteIndex]);
            }
        }
        doOptimizations(tick, everyN, withMutations);
    }

    void Simulation::loop()
    {
        long tick = 0;
        while(true) {
            doLoop(tick, 500);
            usleep(500);
            ++tick;
        }
    }

    void Simulation::regeneratePopulation(bool const withMutations)
    {
        // store all fitness values
        std::vector<std::pair<int, double> > fitnesses;
        fitnesses.reserve(m_popSize);
        auto i = 0;
        for (auto const & agent : m_agents) {
            auto dm = agent.distanceMoved();
            fitnesses.emplace_back(i, dm);
            ++i;
        }

        // sort according to fitness
        std::sort(std::begin(fitnesses), std::end(fitnesses),
                  [](std::pair<int, double> const & a,
                     std::pair<int, double> const & b) {
                      return b.second < a.second;
                  });

        // Store index of the fittest to replace broken agents
        m_eliteIndex = fitnesses[0].first;

        for(auto const & f : fitnesses) {
            std::cout<<f.second<<std::endl;
        }

        // pick the top m_popSize/10 and regen rest of population out of those
        i = 0;
        for (auto & agent : m_agents) {

            // inherit neat genome from fit agent
            if (i != fitnesses[i].first) {
                agent.inheritNeat(m_agents[fitnesses[i].first]);

                // mutate inherited genome
                if(withMutations) {
                    agent.modifyController();
                }  
            }
            // put controller back in basal state
            agent.resetController();
            ++i;
            if (i == (m_popSize/10)) {
                i = 0;
            }
        }

    }

    void Simulation::doOptimizations(long const tick, 
                                     int const everyN, 
                                     bool const withMutations)
    {
        /// How often to 'mutate the controllers'
        if(tick % everyN == 0 && tick > 0) {

            // record distances travelled for each agent
            for (auto & agent : m_agents) {
                agent.recordDistanceMoved();
            }

            regeneratePopulation(withMutations);

            m_animatWorld.randomizePositions(10, 10);

            // set agent starting positions
            for (auto & agent : m_agents) {
                agent.recordStartPosition();
            }
        }
    }

}