/// Copyright (c) 2017 Ben Jones

#include "simulator/Population.hpp"

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

    inline int randomInt(int const popSize)
    {
        return (rand() % static_cast<int>(popSize));
    }

    // Experimental; crossover function not working
    neat::Network offspringNet(std::vector<simulator::Agent> const & agents) 
    {
        auto & candA = agents[randomInt(agents.size())];
        auto & candB = agents[randomInt(agents.size())];
        auto & candC = agents[randomInt(agents.size())];
        auto & candD = agents[randomInt(agents.size())];
        auto dmA = candA.distanceMoved();
        auto dmB = candB.distanceMoved();
        auto dmC = candC.distanceMoved();
        auto dmD = candD.distanceMoved();

        auto & parentA = (dmA > dmB) ? candA : candB;
        auto & parentB = (dmC > dmD) ? candC : candD;

        auto neatA = parentA.getNeatNet();
        auto neatB = parentB.getNeatNet();

        return neatA.crossWith(neatB);
    }
}

namespace simulator {

    Population::Population(int const popSize, model::AnimatWorld & animatWorld)
    : m_popSize(popSize)
    , m_animatWorld(animatWorld)
    , m_eliteIndex(0)
    {
        m_agents.reserve(popSize);
        for(int i = 0;i<popSize;++i){
            m_agents.emplace_back(m_animatWorld.animat(i));
        }
    }

    void Population::update()
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
    }

    void Population::optimize(long const tick, 
                  int const everyN,
                  bool const withMutations)
    {
        /// How often to 'mutate the controllers'
        if(tick % everyN == 0 && tick > 0) {

            // record distances travelled for each agent
            for (auto & agent : m_agents) {
                agent.recordDistanceMoved();
            }

            regenerate(withMutations);

            m_animatWorld.randomizePositions(10, 10);

            // set agent starting positions
            for (auto & agent : m_agents) {
                agent.recordStartPosition();
            }
        }
    }

    void Population::regenerate(bool const withMutations)
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

        // pick the top m_popSize/N and regen rest of population out of those
        i = 0;
        int pick = 0;
        for (auto & agent : m_agents) {
            /*
            if (i == 0) {
                // always preserve elite member
                agent.inheritNeat(m_agents[m_eliteIndex]);
            } else {
                auto offspring = offspringNet(m_agents);
                agent.inheritNeat(offspring);
                agent.modifyController();
            }
            agent.resetController();
            ++i;
*/
            
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
}