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
    neat::Network offspringNet(int const index, std::vector<simulator::Agent> const & agents) 
    {

        // Only cross over most similar
        auto & candA = agents[index];
        auto neatA = candA.getNeatNet();

        auto diff = 10000;
        int i = 0;
        int rem = 0;
        for(auto const & candB : agents) {
            auto neatB = candB.getNeatNet();
            auto d = neatA.measureDifference(neatB);
            if(d < diff && i != index) {
                diff = d;
                rem = i;
            }
            ++i;
        }

        auto & candB = agents[rem];
        auto neatB = candB.getNeatNet();

        //std::cout<<"diff: "<<(neatA.measureDifference(neatB))<<std::endl;

        return neatA.crossWith(neatB);
    }

    double getSharedFitness(int const index, std::vector<simulator::Agent> const & agents)
    {
        // Only cross over most similar
        auto & candA = agents[index];
        auto neatA = candA.getNeatNet();

        auto thresh = 5.0;
        int i = 0;
        for(auto const & candB : agents) {
            auto neatB = candB.getNeatNet();
            auto d = neatA.measureDifference(neatB);
            if(d < thresh) {
                ++i;
            }
        }
        return agents[index].distanceMoved() / i;
        
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
            m_animatWorld.randomizePositionSingleAnimat(i, 10, 10);
            m_agents.emplace_back(m_animatWorld.animat(i));
            m_agents.back().recordStartPosition();
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
                agent.setBad();
                // give animat a fitter genome, the elite one in fact
                // agent.inheritNeat(m_agents[m_eliteIndex]);
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
        std::vector<neat::Network> newPop;
        newPop.push_back(m_agents[m_eliteIndex].getNeatNet());

        for(int i = 1; i < m_popSize; ++i) {
            auto candA = randomInt(m_popSize);
            auto candB = randomInt(m_popSize);
            if(getSharedFitness(candA, m_agents) > getSharedFitness(candB, m_agents)) {
                newPop.push_back(m_agents[candA].getNeatNet());
            } else {
                newPop.push_back(m_agents[candB].getNeatNet());
            }
        }

        i = 0;
        for (auto & agent : m_agents) {
            agent.inheritNeat(newPop[i]);
            // mutate inherited genome
            if(i != 0 && withMutations) {
                agent.modifyController();
            } 
            // put controller back in basal state
            agent.resetController();
            ++i;
        }
    }
}