
#include "Ctrnn.hpp"
#include <algorithm>

namespace ctrnn {

    Ctrnn::Ctrnn(int const nCount,
                 double const neuronTC)
      : m_nCount(nCount)
      , m_neuronTC(neuronTC)
    {
        createCtrnn();
    }

    void Ctrnn::reset()
    {
        std::for_each(std::begin(m_neurons), 
                      std::end(m_neurons), 
                      [](CtrnnNeuron & n) { n.reset(); });

        m_connections.clear();
        m_neurons.clear();
    }

    double Ctrnn::getNeuronMembranePotential(int const n) const
    {
        if (n >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        return m_neurons[n].getMembranePotential();
    }

    double Ctrnn::getNeuronActivation(int const n) const
    {
        if (n >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        return m_neurons[n].activation();
    }

    void Ctrnn::createCtrnn()
    {
        for (int n = 0; n < m_nCount; ++n) {
            m_neurons.emplace_back(m_neuronTC);
            m_connections.emplace_back(std::vector<construct>(m_nCount));
        }
    }

    void
    Ctrnn::connect(int const i, int const j, double const w)
    {
        if (j >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        if (i >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        m_connections[j][i] = {i, j, w}; // a list of pre connections made to post
    }

    void
    Ctrnn::setWeight(int const i, int const j, double const w) 
    {
        if (j >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        if (i >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        m_connections[j][i].w = w;
    }

    void
    Ctrnn::setExternalInput(int const n, double const a)
    {
        if (n >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        m_neurons[n].setExternalInput(a);
    }

    void Ctrnn::zeroInputCurrents()
    {
        std::for_each(std::begin(m_neurons), 
                      std::end(m_neurons),
                      [](CtrnnNeuron & n) { n.setExternalInput(0); });
    }

    void Ctrnn::zeroInputCurrent(int const n)
    {
        if (n >= m_neurons.size()) {
            throw std::runtime_error("Ctrnn: out of bounds");
        }
        m_neurons[n].setExternalInput(0);
    }

    void Ctrnn::update()
    {
        for(auto n = 0; n < m_nCount; ++n) {
            auto & toConnections = m_connections[n];
            m_neurons[n].computeActivation(m_neurons, toConnections);
        }
    }

}
