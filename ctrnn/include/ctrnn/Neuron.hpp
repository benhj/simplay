/// Copyright (c) 2017 Ben Jones
#pragma once

#include "construct.hpp"
#include <vector>
#include <deque>

namespace ctrnn {

    class Neuron
    {
      public:
        explicit Neuron(double const tau);
        void reset();

        void setStartingActivation(double const);
        void setExternalInput(double const input);

        void computeActivation(std::vector<Neuron> & otherNeurons, // optimisation
                               std::vector<construct> & connections);

        double activation() const;
        double oldActivation() const;
        double getMembranePotential() const;

        double sigmoid() const;

        void setTimeConstant(double const tau);

      private:
        void updateU(double const inner);

        /// The 'u' in the CTRNN equation 
        double m_membranePotential;

        /// The neuronal time constant value -- speed of leak
        double m_tau;

        /// The current activation at current timestep
        double m_activation;

        /// The activation at the preceding timestep
        double m_oldActivation;

        /// Input current
        double m_externalInput;
    };

}
