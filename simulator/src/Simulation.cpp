/// Copyright (c) 2017 Ben Jones

#include "simulator/Simulation.hpp"
#include <unistd.h>
#include <algorithm>

namespace {
    using FitnessPair = std::pair<int, double>;
    bool 
    iIsIndexedInTopN(int const i,
                     int const N,
                     std::vector<FitnessPair> const & v) 
    {
        auto begin = std::begin(v);
        auto const end = std::begin(v) + N;
        auto found = std::find_if(begin, end, 
                                  [i](FitnessPair const & fp) {
                                      return i == fp.first;
                                  });
        return (found != end);
    }
}

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
            //usleep(500);
            ++tick;
        }
    }

    void Simulation::regeneratePopulation(bool const withMutations)
    {
        // store all fitness values
        std::vector<FitnessPair> fitnesses;
        fitnesses.reserve(m_popSize);
        int i = 0;
        for (auto const & agent : m_agents) {
            auto dm = agent.distanceMoved();
            fitnesses.emplace_back(i, dm);
            ++i;
        }

        // sort according to fitness
        std::sort(std::begin(fitnesses), std::end(fitnesses),
                  [](FitnessPair const & a,
                     FitnessPair const & b) {
                      return b.second < a.second;
                  });

        // Store index of the fittest to replace broken agents
        m_eliteIndex = fitnesses[0].first;
        std::cout<<m_eliteIndex<<"\t"<<m_agents[m_eliteIndex].distanceMoved()<<std::endl;

        // pick the top m_popSize/10 and regen rest of population out of those
        i = 0;
        int pick = 0;
        for (auto & agent : m_agents) {

            // inherit neat genome from fit agent, but only if
            // the inheritee isn't elite
            if (!iIsIndexedInTopN(i, m_popSize / 10, fitnesses)) {
                agent.inheritNeat(m_agents[fitnesses[pick].first]);

                // mutate inherited genome
                if(withMutations) {
                    agent.modifyController();
                }  

                ++pick;

                if(pick == m_popSize / 10) {
                    pick = 0;
                }

            }
            // put controller back in basal state
            agent.resetController();
            ++i;
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
