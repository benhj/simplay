/// Copyright (c) 2017 Ben Jones

#include "simulator/Agent.hpp"
#include "simulator/CTRNNController.hpp"
#include "neat/MutationParameters.hpp"


namespace simulator {
    Agent::Agent(model::Animat & animat)
      : m_animat(animat)
      , m_neat(4, 1, 20, neat::MutationParameters{0.2, 0.2, 0.2, 0.2}, 6.0)
      , m_controller(std::make_shared<CTRNNController>(animat.getBlockCount(), m_neat))
    {
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
}