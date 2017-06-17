
#include "ctrnn/Neuron.hpp"

#include <cmath>
#include <iostream>
#include <cstdlib>

namespace ctrnn {

    Neuron::Neuron(double const tau) 
      : m_membranePotential(0)
      , m_activation(0)
      , m_oldActivation(0)
      , m_externalInput(0)
      , m_tau(tau)
    {

    }

    void
    Neuron::reset()
    {
        m_membranePotential = 0;
        m_activation = 0;
        m_oldActivation = 0;
        m_externalInput = 0;
    }

    void
    Neuron::setStartingActivation(double const u)
    {
        m_membranePotential += u;
    }

    void
    Neuron::updateU(double const inner)
    {
        m_membranePotential += (inner + m_externalInput - m_membranePotential) / m_tau;
    }

    void Neuron::computeActivation(std::vector<Neuron> & otherNeurons, 
                                        std::vector<construct> & connections)
    {

        // Compute current going into connections
        double inner = 0;
        (void)std::for_each(std::begin(connections), 
                            std::end(connections), 
                            [&](construct const & c) {
                                inner += c.w * otherNeurons[c.pre].oldActivation();
                            });

        // update membrane potential u
        updateU(inner);

        m_oldActivation = m_activation;

        // set the new activation based on newly updated u
        m_activation = tanh(m_membranePotential);
    }

    double Neuron::activation() const 
    {
        return m_activation;
    }

    double Neuron::oldActivation() const  
    {
        return m_oldActivation;
    }

    void Neuron::setExternalInput(double const externalInput)
    {
        m_externalInput = externalInput;
    }

    double Neuron::getMembranePotential() const { return m_membranePotential; }

}
