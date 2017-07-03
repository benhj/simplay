// Copyright (c) 2017 Ben Jones

#pragma once

#include "neat/Connection.hpp"
#include "neat/MutationParameters.hpp"
#include "neat/Node.hpp"
#include <vector>
#include <map>

namespace neat {

    struct InnovationInfo {
        int innovationNumber;
        int preNode;
        int postNode;
        bool enabled;
    };

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

        /// Mutates the network -- modifies weights, adds connections
        /// add nodes in place of connections, modifies the node type etc.
        void mutate();

      private:
        int m_inputCount;
        int m_outputCount;
        int m_maxSize;
        
        // Controls the rate at which the network changes
        MutationParameters m_muts;
        double m_weightInitBound;
        std::vector<Node> m_nodes;
        std::vector<int> m_outputIDs;

        // Tracks innovatations; eases crossover process
        std::map<int, InnovationInfo> m_innovationMap;

        /// Fully connect all inputs to all outputs
        void initNet();

        /// Loops over all connections going into output and
        /// calls addNodeInPlaceOf if mutation probability satisfied
        void addNewNodes();

        /// Add a node in place of connection. That is
        /// A--->B becomes A--->C--->B
        void addNodeInPlaceOf(Connection & con);

        /// All connections going into hidden and output nodes
        /// are perturbed byAmount if probability satisfied
        /// by rate stored in actual connection object.
        void perturbWeights(double const byAmount);

        /// Adds a new connection from an unconnected input node 
        /// to a newly added hidden node
        void addConnectionToHiddenNode();

    };

}