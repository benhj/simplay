/// Copyright (c) 2017 Ben Jones

#include "Agent.hpp"

namespace simulator {
    Agent::Agent(model::Animat & animat, Controller & controller)
      : m_animat(animat)
      , m_controller(controller)
    {
    }

    void Agent::actuate()
    {
        auto blockCount = m_animat.getBlockCount();
        for(int integ = 0; integ < 10; ++integ) {
            for (auto i = 0 ; i < blockCount; ++i) {
                auto outputLeft = m_controller.getLeftMotorOutput(i) * 15;
                auto outputRight = m_controller.getRightMotorOutput(i) * 15;
                m_animat.applyBlockContraction(i, 0, outputLeft);
                m_animat.applyBlockContraction(i, 1, outputRight);
            }
            m_animat.applyWaterForces();    
            m_animat.update();
        }   
        m_controller.update();
    }
}