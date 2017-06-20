// Copyright (c) 2017 Ben Jones

#include "neat/Node.hpp"
#include "neat/Connection.hpp"
#include <cstdlib> // rand()
#include <algorithm>
#include <cmath>
#include <cassert>

namespace {

    double const PI = 3.14159265359;

    neat::NodeFunction initNodeFunction(neat::NodeType const & nodeType)
    {
        if(nodeType == neat::NodeType::Input || nodeType == neat::NodeType::Output) {
            return neat::NodeFunction::HyperbolicTangent;
        } else {
            return static_cast<neat::NodeFunction>(rand() % 8);
        }
    }

    double applyNodeFunction(neat::NodeFunction const & nodeFunction, double const in)
    {
        if (nodeFunction == neat::NodeFunction::HyperbolicTangent) {
            return ::tanh(in);
        } else if (nodeFunction == neat::NodeFunction::Absolute) {
            return std::abs(in);
        } else if (nodeFunction == neat::NodeFunction::Gaussian) {
            return ::exp(-((in*in)/(2*2)));
        } else if (nodeFunction == neat::NodeFunction::Sin) {
            return ::sin(in*((2*PI)/4.0));
        } else if (nodeFunction == neat::NodeFunction::Cos) {
            return ::cos(in*((2*PI)/4.0));
        } else if (nodeFunction == neat::NodeFunction::Clipped) {
            return std::min(std::max(in,-3.0),3.0) / 3.0;
        } else if (nodeFunction == neat::NodeFunction::Step) {
            return (in > 0) - (in < 0);
        } else {
            return in;
        }
    }
}

namespace neat {
    Node::Node(int const index, 
               NodeType const & nodeType,
               double const mutationProbability)
      : m_index(index)
      , m_nodeType(nodeType)
      , m_mutationProbability(mutationProbability)
      , m_nodeFunction(initNodeFunction(nodeType))
      , m_externalInput(0)
    {
        m_incomingConnections.reserve(50);
    }

    Node::Node(Node const & other)
      : m_index(other.m_index)
      , m_nodeType(other.m_nodeType)
      , m_mutationProbability(other.m_mutationProbability)
      , m_nodeFunction(other.m_nodeFunction)
      , m_externalInput(other.m_externalInput)
      , m_incomingConnections()
    {
        m_incomingConnections.reserve(50);
    }

    Node & Node::operator=(Node const & other)
    {
        if (&other == this) {
            return *this;
        }
        m_index = other.m_index;
        m_nodeType = other.m_nodeType;
        m_mutationProbability = other.m_mutationProbability;
        m_nodeFunction = other.m_nodeFunction;
        m_externalInput = other.m_externalInput;
        m_incomingConnections.clear();
        m_incomingConnections.reserve(50);
        return *this;
    }

    void Node::addIncomingConnectionFrom(Node & otherNode,
                                         double const weightBound,
                                         double const mutProb)
    {

        // Nodes can't connect to nodes of same type (or be recurrent)
        if (otherNode.getNodeType() == m_nodeType) {
            return;
        }

        // Sanity A: Input nodes can't having incoming connections
        assert(m_nodeType != NodeType::Input);

        // Sanity B: Output nodes can't connect to hidden nodes
        if (m_nodeType == NodeType::Hidden) {
            assert(otherNode.getNodeType() != NodeType::Output);
        }

        m_incomingConnections.emplace_back(otherNode, 
                                           *this,
                                           weightBound, 
                                           mutProb);
    }

    void Node::addIncomingConnectionFrom(Node & otherNode,
                                         double const weightBound,
                                         double const mutProb,
                                         double const weight)
    {
        // Nodes can't connect to nodes of same type (or be recurrent)
        if (otherNode.getNodeType() == m_nodeType) {
            return;
        }

        // Sanity A: Input nodes can't having incoming connections
        assert(m_nodeType != NodeType::Input);

        // Sanity B: Output nodes can't connect to hidden nodes
        if (m_nodeType == NodeType::Hidden) {
            assert(otherNode.getNodeType() != NodeType::Output);
        }

        m_incomingConnections.emplace_back(otherNode, 
                                           *this, 
                                           weightBound, 
                                           mutProb,
                                           weight);

    }

    void Node::removeIncomingConnectionFrom(int const i)
    {
        auto it = std::find_if(std::begin(m_incomingConnections),
                               std::end(m_incomingConnections),
                               [i](Connection & con) {
                                   return con.getNodeRefA().getIndex() == i;
                               });

        if (it != std::end(m_incomingConnections)) {
            m_incomingConnections.erase(it);
        }
    }

    int Node::getIndex() const
    {
        return m_index;
    }

    NodeType Node::getNodeType() const
    {
        return m_nodeType;
    }

    bool Node::hasConnectionFrom(int const i) const
    {
        return std::find_if(std::begin(m_incomingConnections),
                            std::end(m_incomingConnections),
                            [i](Connection & con) {
                               return con.getNodeRefA().getIndex() == i;
                            }) != std::end(m_incomingConnections);
    }

    double Node::getConnectionWeightFrom(int const i) const
    {
        auto theConnection = std::find_if(std::begin(m_incomingConnections),
                             std::end(m_incomingConnections),
                             [i](Connection & con) {
                                return con.getNodeRefA().getIndex() == i;
                             });

        return theConnection->weight();
    }

    Connection & Node::getConnectionFrom(int const i)
    {
        auto theConnection = std::find_if(std::begin(m_incomingConnections),
                             std::end(m_incomingConnections),
                             [i](Connection & con) {
                                return con.getNodeRefA().getIndex() == i;
                             });
        if (theConnection == std::end(m_incomingConnections)) {
            throw std::logic_error("No connection from node");
        }
        return *theConnection;
    }

    void Node::perturbNodeFunction()
    {
        if (((double) rand() / (RAND_MAX)) < m_mutationProbability) {
            m_nodeFunction = initNodeFunction(m_nodeType);
        }
    }

    void Node::setExternalInput(double const externalInput)
    {
        m_externalInput = externalInput;
    }

    double Node::getOutput() const
    {
        // back-tracks over all nodes to get final output
        double accumulator = 0;
        for (auto & con : m_incomingConnections) {
            auto & nodeRef = con.getNodeRefA();
            accumulator += nodeRef.getOutput() * con.weight();
        }
        accumulator += m_externalInput;
        return applyNodeFunction(m_nodeFunction, accumulator);
    }

    void Node::perturbIncomingWeights(double const byAmount)
    {
        for (auto & con : m_incomingConnections) {
            con.perturbWeight(byAmount);
        }
    }
}