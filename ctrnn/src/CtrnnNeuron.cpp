
#include "CtrnnNeuron.hpp"

#include <cmath>
#include <iostream>
#include <cstdlib>

namespace ctrnn {

    CtrnnNeuron::CtrnnNeuron(double const tau) 
      : m_membranePotential(0)
      , m_activation(0)
      , m_oldActivation(0)
      , m_externalInput(0)
      , m_tau(tau)
    {

    }

    void
    CtrnnNeuron::reset()
    {
        m_membranePotential = 0;
        m_activation = 0;
        m_oldActivation = 0;
        m_externalInput = 0;
    }

    void
    CtrnnNeuron::setStartingActivation(double const u)
    {
        m_membranePotential += u;
    }

    void
    CtrnnNeuron::updateU(double const inner)
    {
        m_membranePotential += (inner + m_externalInput - m_membranePotential) / m_tau;
    }

    void CtrnnNeuron::computeActivation(std::vector<CtrnnNeuron> & otherNeurons, 
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

    double CtrnnNeuron::activation() const 
    {
        return m_activation;
    }

    double CtrnnNeuron::oldActivation() const  
    {
        return m_oldActivation;
    }

    void CtrnnNeuron::setExternalInput(double const externalInput)
    {
        m_externalInput = externalInput;
    }

    double CtrnnNeuron::getMembranePotential() const { return m_membranePotential; }

}
