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

    int getSegmentCount(int const index, std::vector<simulator::Agent> & agents)
    {
        auto neat = agents[index].getNeatNet();
        auto output = neat.getOutput(2);
        output += 1;
        auto segments = output * 5.0;
        if(segments > 12) {
            segments = 12;
        }
        return (int)(segments);
    }
}

namespace simulator {

    Population::Population(int const popSize, model::AnimatWorld & animatWorld)
    : m_popSize(popSize)
    , m_animatWorld(animatWorld)
    , m_eliteIndex(0)
    , m_evoOn(true)
    {
        m_agents.reserve(popSize);
        for(int i = 0;i<popSize;++i){
            m_animatWorld.randomizePositionSingleAnimat(i, 10, 10);
            m_agents.emplace_back(m_animatWorld.animat(i));
            m_agents.back().recordStartPosition();
        }
    }

    void Population::activateEvolution()
    {
        m_evoOn = true;
    }
    void Population::deactivateEvolution()
    {
        m_evoOn = false;
    }

    std::vector<simulator::Agent> & Population::getAgents()
    {
        return m_agents;
    }

    void Population::update(long const tick, 
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

            if(agent.update(m_agents) == -1) {

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

                // check bounds
                m_animatWorld.translateIfOutOfBounds(p, 200, 200);

                agent.recordStartPosition();
            } else {
                agent.incrementAge();
            }
            ++p;
        }

        // Probabilitisticazlly choose parent to generate offspring scaled
        // according to the overall best adjusted fitness
        if(m_evoOn) {
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
                m_agents[worstIndex].inheritNeat(m_agents[choice]);
                m_agents[worstIndex].mutateNeat();
                // Mutating the NEAT architecture can result in a
                // different number of body segments meaning that
                // the animat needs to be reconstructed.
                /*
                auto const oldSegmentCount = m_animatWorld.animat(worstIndex)
                                             ->getBlockCount();
                auto const segments = getSegmentCount(worstIndex, m_agents);
                if(segments != oldSegmentCount) {
                    m_animatWorld.reconstructAnimat(worstIndex, segments);
                    m_animatWorld.randomizePositionSingleAnimat(worstIndex, 10, 10);
                }*/
                m_agents[worstIndex].resetController();
                m_animatWorld.incrementOptimizationCount();
            }
        }
    }
}