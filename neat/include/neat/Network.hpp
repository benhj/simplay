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
        double weight;
        bool enabled;
    };

    class Network
    {
      public:
        using InnovationMap = std::map<int, InnovationInfo>;
        Network(int const inputCount, 
                int const outputCount,
                int const maxSize,
                MutationParameters const & mutationParams,
                double const weightInitBound,
                InnovationMap const & innovMap = InnovationMap());

        Network(Network const & other);
        Network & operator=(Network const & other);

        void setInput(int const i, double const value);
        double getOutput(int const i) const;

        /// Mutates the network -- modifies weights, adds connections
        /// add nodes in place of connections, modifies the node type etc.
        void mutate();

        /// Performs a cross-over with another network. Note
        /// the other network is *always* considered the fitter
        /// of the two. This should be taken into account when
        /// calling the given function.
        Network crossWith(Network const & other) const;

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
        InnovationMap m_innovationMap;

        // Keeps track of new structural innovations
        static int GLOBAL_INNOVATION_NUMBER;

        /// Fully connect all inputs to all outputs, or,
        /// based on innovation map when map non-empty
        void initNet();

        /// When a new network is constructed, we need an
        /// initial set of input and hidden nodes
        void assembleInitialInputAndOutputNodes();

        /// When initially constructed, there will be full
        /// connectivity from input to output nodes
        void assembleInitialInputToOutputConnectivity();

        /// During cross-over, the new network is to be
        /// constructed from a pre-computed innovation map
        void assembleFromInnovationMap();

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
        /// to a newly added hidden node, or an existing output node
        void addConnectionToHiddenOrOutputNode();

        /// Mutates node function type
        void perturbNodeFunctions();

    };

}