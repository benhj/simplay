// Copyright (c) 2017 Ben Jones

#pragma once

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
                double const nodeAdditionProb,
                double const nodeFunctionChangeProb,
                double const weightChangeProb,
                double const weightInitBound);

        Network(Network const & other);
        Network & operator=(Network const & other);

        void setInput(int const i, double const value);
        double getOutput(int const i) const;


      private:
        int m_inputCount;
        int m_outputCount;
        int m_maxSize;
        double m_nodeAdditionProb;
        double m_nodeFunctionChangeProb;
        double m_weightChangeProb;
        double m_weightInitBound;
        std::vector<Node> m_nodes;

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