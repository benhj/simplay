// Copyright (c) 2017 Ben Jones

#include "neat/Connection.hpp"
#include "neat/Node.hpp"
#include <cstdlib>

namespace {
    double initWeight(double const weightBound)
    {
        auto weight = ((double) rand() / (RAND_MAX)) * weightBound;
        if ((rand() / RAND_MAX) < 0.5) {
            weight = -weight;
        }
        return weight;
    }
}

namespace neat {
    Connection::Connection(Node & nodeA,
                           Node & nodeB,
                           double const weightBound, 
                           double const mutationProbability)
      : m_nodeA(nodeA)
      , m_nodeB(nodeB)
      , m_mutationProbability(mutationProbability)
      , m_weight(initWeight(weightBound))
    {
    }

    Connection::Connection(Connection const & other)
      : m_nodeA(other.m_nodeA)
      , m_nodeB(other.m_nodeB)
      , m_mutationProbability(other.m_mutationProbability)
      , m_weight(other.m_weight)
    {
    }

    Connection::Connection(Connection && other)
      : m_nodeA(other.m_nodeA)
      , m_nodeB(other.m_nodeB)
      , m_mutationProbability(std::move(other.m_mutationProbability))
      , m_weight(std::move(other.m_weight))
    {
    }

    Connection & Connection::operator=(Connection const & other)
    {
        if (&other == this) {
            return *this;
        }
        m_nodeA = other.m_nodeA;
        m_nodeB = other.m_nodeB;
        m_mutationProbability = other.m_mutationProbability;
        m_weight = other.m_weight;
        return *this;
    }

    Connection & Connection::operator=(Connection && other)
    {
        if (&other == this) {
            return *this;
        }
        m_nodeA = other.m_nodeA;
        m_nodeB = other.m_nodeB;
        m_mutationProbability = std::move(other.m_mutationProbability);
        m_weight = std::move(other.m_weight);
        return *this;
    }

    double Connection::weight() const
    {
        return m_weight;
    }

    /// Mutates the weight value
    void Connection::perturbWeight(double const weightStep)
    {
        if (((double) rand() / (RAND_MAX)) < m_mutationProbability) {
            m_weight += initWeight(weightStep);
        }
    }

    Node & Connection::getNodeRefA() { return m_nodeA; }
    Node & Connection::getNodeRefB() { return m_nodeB; }
}