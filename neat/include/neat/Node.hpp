// Copyright (c) 2017 Ben Jones

#pragma once

#include "NodeType.hpp"
#include "NodeFunction.hpp"
#include <vector>

namespace neat {

    class Connection;

    class Node
    {
      public:
        Node(int const index, 
             NodeType const & nodeType,
             double const mutationProbability);
        Node() = delete;

        /// Need to ensure that when nodes are copied,
        /// the vector of incoming connections which has
        /// references to other nodes gets omitted
        Node(Node const & other);
        Node & operator=(Node const & other);

        /// Adds a connection from another node
        void addIncomingConnectionFrom(Node & otherNode,
                                       double const weightBound,
                                       double const mutProb);

        /// Adds a connection from another node but with a weight
        void addIncomingConnectionFrom(Node & otherNode,
                                       double const weightBound,
                                       double const mutProb,
                                       double const weight);

        /// For removing a connection
        void removeIncomingConnectionFrom(int const i);

        /// Updates the type of node with probability
        void perturbNodeFunction();

        /// Perturb incoming weights. The amount by which
        /// weights are perturbed will be determined by the
        /// connection's weight mutation probability.
        void perturbIncomingWeights(double const byAmount);

        /// The input to this node not coming from another node
        void setExternalInput(double const externalInput);

        /// Will back-track to inputs to figure out what
        /// final output current should be
        double getOutput() const;

        /// Retrieves the classic i,j type index of this node
        int getIndex() const;

        NodeType getNodeType() const;

        /// Indicates if this has connection from node i
        bool hasConnectionFrom(int const i) const;

      private:

        /// Indexes node in typical matrix i,j fashion
        int m_index;

        /// Input, hidden or output
        NodeType m_nodeType;

        /// To be used when the node type is perturbed
        double m_mutationProbability;

        /// The node function type(guassian, sigmoidal, tan, etc)
        NodeFunction m_nodeFunction;

        /// Describes all nodes connected to this node
        mutable std::vector<Connection> m_incomingConnections;

        /// Input going into the node not coming
        /// from another node.
        double m_externalInput;
    };
}