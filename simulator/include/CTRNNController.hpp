/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include "Ctrnn.hpp"
#include "NeuralSubstrate.hpp"
#include "neat/Network.hpp"
#include <cstdlib>

namespace simulator {
    class CTRNNController : public Controller
    {
      public:
        explicit CTRNNController(int const blockCount,
                                 neat::Network & neatNet)
          : m_ctrnn(blockCount * 4, 10.0)
        {
            model::NeuralSubstrate neuralSubstrate(blockCount);

            int const nodeCount = blockCount * 4;

            for(int i = 0 ; i < nodeCount; ++i) {
                for(int j = 0; j < nodeCount; ++j) {
                    if (i != j && i >= nodeCount / 2) {
                        auto ix = neuralSubstrate.getX(i);
                        auto iy = neuralSubstrate.getY(i);
                        auto jx = neuralSubstrate.getX(j);
                        auto jy = neuralSubstrate.getY(j);
                        neatNet.setInput(0, ix);
                        neatNet.setInput(1, iy);
                        neatNet.setInput(2, jx);
                        neatNet.setInput(3, jx);
                        m_ctrnn.connect(i, j, neatNet.getOutput(0));
                    }
                }
            }
            m_ctrnn.setExternalInput(nodeCount / 2, 1.0);
            m_ctrnn.setExternalInput(nodeCount - blockCount, 1.0);
            m_ctrnn.update();
        }

        CTRNNController() = delete;

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