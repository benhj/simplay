// Copyright (c) 2017 Ben Jones

#pragma once

namespace neat {

    class Node;

    class Connection
    {
      public:  
        Connection(Node & nodeA,
                   Node & nodeB,
                   double const weightBound, 
                   double const mutationProbability,
                   int const innovationNumber);

        Connection(Node & nodeA,
                   Node & nodeB,
                   double const weightBound, 
                   double const mutationProbability,
                   int const innovationNumber,
                   double const weight);

        // Since we have references as member vars,
        // we need explicit copy and assignment operators.
        // Applying rule of 4, also have the move equivalents.
        Connection(Connection const & other);
        Connection(Connection && other);
        Connection & operator=(Connection const & other);
        Connection & operator=(Connection && other);

        /// Mutates the weight value
        void perturbWeight(double const weightStep);

        Node & getNodeRefA();
        Node & getNodeRefB();

        double weight() const;

        int getInnovationNumber() const;

      private:
        /// The connection end-points
        Node & m_nodeA;
        Node & m_nodeB;

        /// Probability of weight changing when updated
        double m_mutationProbability;

        /// The unique innovation number of this connection
        int m_innovationNumber;

        /// The actual connection weight between nodes
        double m_weight;
    };
}