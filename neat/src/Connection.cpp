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
                           double const mutationProbability,
                           int const innovationNumber)
      : m_nodeA(nodeA)
      , m_nodeB(nodeB)
      , m_mutationProbability(mutationProbability)
      , m_innovationNumber(innovationNumber)
      , m_weight(initWeight(weightBound))
    {
    }

    Connection::Connection(Node & nodeA,
                           Node & nodeB,
                           double const weightBound, 
                           double const mutationProbability,
                           int const innovationNumber,
                           double const weight)
      : m_nodeA(nodeA)
      , m_nodeB(nodeB)
      , m_mutationProbability(mutationProbability)
      , m_innovationNumber(innovationNumber)
      , m_weight(weight)
    {
    }

    Connection::Connection(Connection const & other)
      : m_nodeA(other.m_nodeA)
      , m_nodeB(other.m_nodeB)
      , m_mutationProbability(other.m_mutationProbability)
      , m_innovationNumber(other.m_innovationNumber)
      , m_weight(other.m_weight)
    {
    }

    Connection::Connection(Connection && other)
      : m_nodeA(other.m_nodeA)
      , m_nodeB(other.m_nodeB)
      , m_mutationProbability(std::move(other.m_mutationProbability))
      , m_innovationNumber(std::move(other.m_innovationNumber))
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
        m_innovationNumber = other.m_innovationNumber;
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
        m_innovationNumber = std::move(other.m_innovationNumber);
        m_weight = std::move(other.m_weight);
        return *this;
    }

    double Connection::weight() const
    {
        return m_weight;
    }

    int Connection::getInnovationNumber() const
    {
        return m_innovationNumber;
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