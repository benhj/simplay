/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include "Ctrnn.hpp"
#include <cstdlib>

namespace simulator {
    class CTRNNController : public Controller
    {
      public:
        CTRNNController()
          : m_ctrnn(32, 10.0)
        {
           for(int i = 0 ; i < 32; ++i) {
               for(int j = 0; j < 32; ++j) {
                   if (i != j && i >= 16) {
                       auto val = ((double) rand() / (RAND_MAX)) * 2.0;
                       if (((double) rand() / (RAND_MAX)) < 0.5) {
                           val = -val;
                       }
                       m_ctrnn.connect(i, j, val);
                   }
               }
           }
           m_ctrnn.setExternalInput(16, 1.0);
           m_ctrnn.setExternalInput(24, 1.0);
           m_ctrnn.update();
        }

        double getLeftMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronActivation(i);
        }
        double getRightMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronActivation(i + 8);
        }
        void update() override
        {
            m_ctrnn.update();
        }
      private:
        mutable ctrnn::Ctrnn m_ctrnn;
    };
}