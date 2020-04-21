/// Copyright (c) 2017 Ben Jones

#include "simulator/Agent.hpp"
#include "simulator/CTRNNController.hpp"
#include "neat/MutationParameters.hpp"

namespace {
    int const NEAT_INPUTS = 7;
    int const NEAT_OUTPUTS = 2;
    int const MAX_NEAT_NODES = 12;
    double const NEAT_WEIGHT_BOUND = 30.0;
    neat::MutationParameters NEAT_MUTS{0.2,  // node addition
                                       0.2,  // node function change
                                       0.5,  // weight change
                                       0.2}; // connection addition
}

namespace simulator {
    Agent::Agent(std::shared_ptr<model::Animat> animat)
      : m_animat(std::move(animat))
      , m_neat(NEAT_INPUTS, 
               NEAT_OUTPUTS, 
               MAX_NEAT_NODES, 
               NEAT_MUTS,
               NEAT_WEIGHT_BOUND)
      , m_controller(std::make_shared<CTRNNController>(m_animat->getBlockCount(), m_neat))
      , m_startPosition{0,0,0}
      , m_distanceMoved(0)
      , m_bad(false)
      , m_age(0)
      , m_adjustedFitness(0)
    {
        // set where the animat currently is in the world
        // will be used as a basis for computing distance moved
        // recordStartPosition();
    }

    void Agent::setAdjustedFitness(double const adjustedFitness)
    {
        m_adjustedFitness = adjustedFitness;
    }

    double Agent::getAdjustedFitness() const
    {
        return m_adjustedFitness;
    }

    long Agent::getAge() const
    {
        return m_age;
    }
    void Agent::resetAge()
    {
        m_age = 0;
        m_bad = false;
    }
    void Agent::incrementAge()
    {
        ++m_age;
    }

    void Agent::updateSpeciesColour(double r, double g, double b)
    {
        m_animat->updateSpeciesColour(r, g, b);
    }

    model::SpeciesColour Agent::getSpeciesColour() const
    {
        return m_animat->getSpeciesColour();
    }

    int Agent::update()
    {
        auto blockCount = m_animat->getBlockCount();
        for(int integ = 0; integ < 10; ++integ) {
            for (auto i = 0 ; i < blockCount; ++i) {
                auto outputLeft = m_controller->getLeftMotorOutput(i) * 20;
                auto outputRight = m_controller->getRightMotorOutput(i) * 20;
                m_animat->applyBlockContraction(i, 0, outputLeft);
                m_animat->applyBlockContraction(i, 1, outputRight);
            }
            m_animat->applyWaterForces();    
            m_animat->update();
            if (m_animat->broke()) {
                return -1;
            }
        }   
        m_controller->update();
        return 0;
    }

    void Agent::resetNeat()
    {
        m_neat = neat::Network(NEAT_INPUTS, 
                               NEAT_OUTPUTS, 
                               MAX_NEAT_NODES, 
                               NEAT_MUTS,
                               NEAT_WEIGHT_BOUND);
    }

    void Agent::inheritNeat(Agent const & other)
    {
        m_neat = other.m_neat;
        m_controller->set();
    }

    void Agent::inheritNeat(neat::Network const & net)
    {
        m_neat = net;
        m_controller->set();
    }

    void Agent::mutateNeat()
    {
        // During mutation, it is possible for new
        // innovations to appear. Whenever this happens
        // the agent becomes a new species which is
        // visualized using colour.
        if(m_neat.mutate()) {
            auto r = ::rand() / double(RAND_MAX);
            auto g = ::rand() / double(RAND_MAX);
            auto b = ::rand() / double(RAND_MAX);
            r *= 100;
            g *= 100;
            b *= 100;
            r += 155;
            g += 155;
            b += 155;
            updateSpeciesColour(r, g, b);
        }
    }

    void Agent::resetController()
    {
        m_controller.reset();
        m_controller = std::make_shared<CTRNNController>(m_animat->getBlockCount(), m_neat);
    }

    void Agent::recordStartPosition()
    {
        m_startPosition = m_animat->getCentralPoint().first;
    }

    void Agent::recordDistanceMoved() 
    {
        if(m_bad) {
            m_distanceMoved = 0;
        } else {
            m_distanceMoved = (m_animat->getCentralPoint().first).distance(m_startPosition);
        }
    }

    double Agent::distanceMoved() const
    {
        return m_distanceMoved;
    }

    neat::Network Agent::getNeatNet() const
    {
        return m_neat;
    }

    void Agent::setBad()
    {
        m_bad = true;
    }
}
