/*
  Copyright (c) 2012-2017, Benjamin H.D. Jones
  All rights reserved.
*/


#pragma once

/**
 * Header file for analog leaky integrator network implementation used
 * for the control of the animat
 *
 * Ben Jones 2010
 *
 */


#include "construct.hpp"
#include "Neuron.hpp"

namespace ctrnn {

    class Network
    {
      public:

        Network(int const nCount, double const neuronTC);

        /// Zeros out the network
        void reset();

        double getNeuronMembranePotential(int const n) const;

        double getNeuronActivation(int const n) const;

        double getNeuronSigmoid(int const n) const;

        /// Create a fully connected network. Connectivity
        /// will be determined by weight
        void createNetwork();

        void connect(int const i, int const j, double const w);

        void setWeight(int const i, int const j, double const w);
        void setTimeConstantForNeuron(int const n, double const tau);

        /// Sets the input current for a given neuron
        void setExternalInput(int const n, double const a);

        /// Zero out all network input currents
        void zeroInputCurrents();
        void zeroInputCurrent(int const n);

        /// Perform a single integration step
        void update();

      private:

        /// Total number of neurons used by the network
        int m_nCount;

        /// Neuronal time constant
        double m_neuronTC;

        /// The network's neurons
        std::vector<Neuron> m_neurons;

        /// Basically the weight matrix
        std::vector<std::vector<construct>> m_connections;
        

    };
}


