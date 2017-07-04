/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include "ctrnn/Network.hpp"
#include "model/NeuralSubstrate.hpp"
#include "neat/Network.hpp"
#include "neat/Node.hpp"
#include "neat/Connection.hpp"
#include <cstdlib>

namespace simulator {
    class CTRNNController : public Controller
    {
      public:
        explicit CTRNNController(int const blockCount,
                                 neat::Network & neatNet)
          : m_blockCount(blockCount)
          , m_neatNet(neatNet)
          , m_ctrnn(blockCount * 4, 15.0)
        {
            set();
        }

        CTRNNController() = delete;

        void set() override
        {
            model::NeuralSubstrate neuralSubstrate(m_blockCount);

            int const nodeCount = m_blockCount * 4;

            for(int i = 0 ; i < nodeCount; ++i) {
                for(int j = 0; j < nodeCount; ++j) {
                    if (i != j && i >= nodeCount / 2) {
                        auto ix = neuralSubstrate.getX(i);
                        auto iy = neuralSubstrate.getY(i);
                        auto jx = neuralSubstrate.getX(j);
                        auto jy = neuralSubstrate.getY(j);
                        m_neatNet.setInput(0, ix);
                        m_neatNet.setInput(1, iy);
                        m_neatNet.setInput(2, jx);
                        m_neatNet.setInput(3, jy);
                        m_ctrnn.connect(i, j, m_neatNet.getOutput(0));
                    }
                }
            }
            m_ctrnn.setExternalInput(nodeCount / 2, 1.0);
            m_ctrnn.setExternalInput(nodeCount - m_blockCount, 1.0);
            m_ctrnn.update();
        }

        double getLeftMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronActivation(i);
        }
        double getRightMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronActivation(i + m_blockCount);
        }
        void update() override
        {
            m_ctrnn.update();
        }
      private:
        int const m_blockCount;
        neat::Network & m_neatNet;
        mutable ctrnn::Network m_ctrnn;

    };
}