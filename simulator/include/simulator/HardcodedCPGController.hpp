/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include <cstdlib>
#include <vector>

namespace simulator {

    struct Actuator {
        long start;
        long counter;
        bool output;
        long waveLength;
    };

    class HardcodedCPGController : public Controller
    {
      public:
        HardcodedCPGController(int const phaseOffset, 
                               int const waveLength,
                               int const segs)
          : m_phaseOffset(phaseOffset)
          , m_waveLength(waveLength)
          , m_segs(segs)
        {
            for (auto i = 0; i < segs; ++i) {

                
                Actuator left{(i-(segs/2)) * phaseOffset, 0, false, waveLength};
                m_leftActuators.push_back(left);
                

                Actuator right{(i+(segs/2)) * phaseOffset, 0, false, waveLength};
                
                m_rightActuators.push_back(right);
            }
        }

        double getLeftMotorOutput(int const i) const override
        {
            return m_leftActuators[i].output;
        }
        double getRightMotorOutput(int const i) const override
        {
            return m_rightActuators[i].output;
        }
        void update() override
        {
            static long counter = 0;
            for (auto i = 0; i < m_segs; ++i) {
                auto & left = m_leftActuators[i];
                auto & right = m_rightActuators[i];
                if (counter > left.start && left.counter % left.waveLength == 0) {
                    left.output = !left.output;
                } 
                ++left.counter;

                if (counter > right.start && right.counter % right.waveLength == 0) {
                    right.output = !right.output;
                }
                ++right.counter;

            }
            ++counter;
        }

        void set() override {}

      private:
        int m_phaseOffset;
        int m_waveLength;
        int m_segs;
        std::vector<Actuator> m_leftActuators;
        std::vector<Actuator> m_rightActuators;


    };
}