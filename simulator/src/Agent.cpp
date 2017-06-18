/// Copyright (c) 2017 Ben Jones

#include "simulator/Agent.hpp"
#include "simulator/CTRNNController.hpp"
#include "neat/MutationParameters.hpp"

namespace {
    int const NEAT_INPUTS = 4;
    int const NEAT_OUTPUTS = 1;
    int const MAX_NEAT_NODES = 20;
    double const NEAT_WEIGHT_BOUND = 6.0;
    neat::MutationParameters NEAT_MUTS{0.05, 0.005, 0.05, 0.2};
}

namespace simulator {
    Agent::Agent(model::Animat & animat)
      : m_animat(animat)
      , m_neat(NEAT_INPUTS, 
               NEAT_OUTPUTS, 
               MAX_NEAT_NODES, 
               NEAT_MUTS,
               NEAT_WEIGHT_BOUND)
      , m_controller(std::make_shared<CTRNNController>(animat.getBlockCount(), m_neat))
      , m_distanceMoved(0)
    {
        // set where the animat currently is in the world
        // will be used as a basis for computing distance moved
        recordStartPosition();
    }

    int Agent::update()
    {
        auto blockCount = m_animat.getBlockCount();
        for(int integ = 0; integ < 10; ++integ) {
            for (auto i = 0 ; i < blockCount; ++i) {
                auto outputLeft = m_controller->getLeftMotorOutput(i) * 15;
                auto outputRight = m_controller->getRightMotorOutput(i) * 15;
                m_animat.applyBlockContraction(i, 0, outputLeft);
                m_animat.applyBlockContraction(i, 1, outputRight);
            }
            m_animat.applyWaterForces();    
            m_animat.update();
            if (m_animat.broke()) {
                return -1;
            }
        }   
        m_controller->update();
        return 0;
    }

    void Agent::inheritNeat(Agent const & other)
    {
        m_neat = other.m_neat;
        m_controller->set();
    }

    void Agent::modifyController()
    {
        m_neat.mutate();
        m_controller->set();
    }

    void Agent::recordStartPosition()
    {
        m_startPosition = m_animat.getCentralPoint().first;
    }

    void Agent::recordDistanceMoved() 
    {
        m_distanceMoved = (m_animat.getCentralPoint().first).distance(m_startPosition);
    }

    double Agent::distanceMoved() const
    {
        return m_distanceMoved;
    }
}