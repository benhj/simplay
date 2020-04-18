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
        return neatA.crossWith(neatB);
    }

    double getSharedFitness(simulator::Agent & candidate, std::vector<simulator::Agent> & agents)
    {
        // Only cross over most similar
        auto neatA = candidate.getNeatNet();

        // Species colour-coded (r,g,b);
        auto speciesColour = candidate.getSpeciesColour();
        auto const r = speciesColour.R;
        auto const g = speciesColour.G;
        auto const b = speciesColour.B;

        auto thresh = 1;
        int i = 0;
        for(auto & candB : agents) {
            auto neatB = candB.getNeatNet();
            auto d = neatA.measureDifference(neatB);
            if(d < thresh) {
                candB.updateSpeciesColour(r, g, b);
                ++i;
            }
        }
        auto const adjusted = candidate.distanceMoved() / (double)i;
        return adjusted;
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
        /*
        for (int p = 0; p < m_popSize; ++p) {
            auto & agent = m_agents[p];

            // if physics broke, reinit position in world
            if(agent.update() == -1) {
                m_animatWorld.randomizePositionSingleAnimat(p, 10, 10);
                agent.recordStartPosition();
                agent.resetController();
                agent.setBad();
                agent.resetAge();
                // give animat a fitter genome, the elite one in fact
                // agent.inheritNeat(m_agents[m_eliteIndex]);
            }
        }*/
    }

    void Population::optimize(long const tick, 
                  int const everyN,
                  bool const withMutations)
    {

        int p = 0;
        double best = 0.0;
        double worst = 10000;
        int worstIndex = -1;
        int bestIndex = -1;
        double totalAdjusted = 0;
        for (auto & agent : m_agents) {

            if(agent.update() == -1) {

                // Reinitialize the underlying neat genome
                m_animatWorld.randomizePositionSingleAnimat(p, 10, 10);
                agent.recordStartPosition();
                agent.resetController();
                agent.setBad();
            }

            agent.recordDistanceMoved();

            // old age
            // choose another population member at random
            // and replace this one if chosen one is fitter.
            if(agent.getAge() >= 20) {
                auto adjustedFitness = getSharedFitness(agent, m_agents);
                agent.setAdjustedFitness(adjustedFitness);
                if(adjustedFitness > best) {
                    best = adjustedFitness;
                    bestIndex = p;
                }
                if(adjustedFitness < worst) {
                    worst = adjustedFitness;
                    worstIndex = p;
                }
                agent.resetAge();
                agent.recordStartPosition();
            } else {
                agent.incrementAge();
            }
            ++p;
        }

        // Probabilitisticazlly choose parent to generate offspring scaled
        // according to the overall best adjusted fitness
        std::vector<FitnessPair> fitnesses;
        fitnesses.reserve(m_popSize);
        int i = 0;
        for (auto const & agent : m_agents) {
            auto f = agent.getAdjustedFitness();
            fitnesses.emplace_back(i, f);
            ++i;
        }

        // sort according to fitness (lowest to highest)
        std::sort(std::begin(fitnesses), std::end(fitnesses),
                  [](FitnessPair const & a,
                     FitnessPair const & b) {
                      return b.second > a.second;
                  });

        auto arand = ::rand() / double(RAND_MAX);
        int choice = 0;
        for (auto const & fitness : fitnesses) {
            if(arand <= (fitness.second / best)) {
                choice = fitness.first;
                break;
            }
        }

        if(bestIndex > -1 && worstIndex > -1 && worstIndex != choice) {
            //std::cout<<m_agents[worstIndex].getAdjustedFitness()<<"\t"<<m_agents[choice].getAdjustedFitness()<<std::endl;
            m_agents[worstIndex].inheritNeat(m_agents[choice]);
            m_agents[worstIndex].modifyController();
            m_agents[worstIndex].resetController();
        }

        /// How often to 'mutate the controllers'
        /*
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
        */
    }

    void Population::regenerate(bool const withMutations)
    {
        /*
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
        */
    }
}