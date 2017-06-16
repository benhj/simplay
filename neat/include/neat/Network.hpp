// Copyright (c) 2017 Ben Jones

#pragma once

#include "neat/MutationParameters.hpp"
#include <vector>

namespace neat {

    class Node;
    class Connection;

    class Network
    {
      public:
        Network(int const inputCount, 
                int const outputCount,
                int const maxSize,
                MutationParameters const & mutationParams,
                double const weightInitBound);

        Network(Network const & other);
        Network & operator=(Network const & other);

        void setInput(int const i, double const value);
        double getOutput(int const i) const;


      private:
        int m_inputCount;
        int m_outputCount;
        int m_maxSize;
        
        // Controls the rate at which the network changes
        MutationParameters m_muts;
        double m_weightInitBound;
        std::vector<Node> m_nodes;
        std::vector<int> m_outputIDs;

        /// Fully connect all inputs to all outputs
        void initNet();

        /// Add a node in place of connection. That is
        /// A--->B becomes A--->C--->B
        void addNodeInPlaceOf(Connection & con);

        /// All connections going into hidden and output nodes
        /// are perturbed byAmount if probability satisfied
        /// by rate stored in actual connection object.
        void perturbWeights(double const byAmount);
    };

}