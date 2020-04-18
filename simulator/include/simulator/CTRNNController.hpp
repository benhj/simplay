/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include "ctrnn/Network.hpp"
#include "model/NeuralSubstrate.hpp"
#include "neat/Network.hpp"
#include "neat/Node.hpp"
#include "neat/Connection.hpp"
#include <cstdlib>
#include <cmath>

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

            // Set weights
            for(int i = 0 ; i < nodeCount; ++i) {
                auto ix = neuralSubstrate.getX(i) * 10;
                auto iy = neuralSubstrate.getY(i) * 10;
                m_neatNet.setInput(0, ix);
                m_neatNet.setInput(1, iy);
                for(int j = 0; j < nodeCount; ++j) {
                    if (i != j && i >= nodeCount / 2) {
                        auto jx = neuralSubstrate.getX(j) * 10;
                        auto jy = neuralSubstrate.getY(j) * 10;
                        m_neatNet.setInput(2, jx);
                        m_neatNet.setInput(3, jy);
                        m_ctrnn.connect(i, j, m_neatNet.getOutput(0) * 50.0);
                    }
                }

                // Set time constants
                m_neatNet.setInput(0, 0);
                m_neatNet.setInput(1, 0);
                m_neatNet.setInput(2, 0);
                m_neatNet.setInput(3, 0);
                m_neatNet.setInput(4, ix);
                m_neatNet.setInput(5, iy);
                m_neatNet.setInput(6, 10);
                auto tau = fabs(m_neatNet.getOutput(1));
                tau *= 20.0;
                tau += 20.0;
                m_ctrnn.setTimeConstantForNeuron(i, tau);
            }

            m_ctrnn.setExternalInput(nodeCount / 2, 50.0);
            m_ctrnn.setExternalInput(nodeCount - m_blockCount, 50.0);
            m_ctrnn.update();
        }

        double getLeftMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronSigmoid(i);
        }
        double getRightMotorOutput(int const i) const override
        {
            return m_ctrnn.getNeuronSigmoid(i + m_blockCount);
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