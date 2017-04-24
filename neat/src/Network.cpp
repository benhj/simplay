// Copyright (c) 2017 Ben Jones

#include "neat/Network.hpp"
#include "neat/NodeType.hpp"
#include "neat/Node.hpp"
#include "neat/Connection.hpp"

namespace {
    void restoreConnectivity(std::vector<neat::Node> const & oldNodes,
                             std::vector<neat::Node> & newNodes,
                             double const weightInitBound,
                             double const weightChangeProb)
    {
        for (auto i = 0; i < oldNodes.size(); ++i) {
            for (auto j = 0; j < oldNodes.size(); ++j) {
                if (i == j) { continue; }
                if (oldNodes[j].hasConnectionFrom(i)) {
                    newNodes[j].addIncomingConnectionFrom(newNodes[i], 
                                                          weightInitBound,
                                                          weightChangeProb);
                }
            }
        }
    }
}

namespace neat {
    Network::Network(int const inputCount, 
                     int const outputCount,
                     int const maxSize,
                     double const nodeAdditionProb,
                     double const nodeFunctionChangeProb,
                     double const weightChangeProb,
                     double const weightInitBound)
      : m_inputCount(inputCount)
      , m_outputCount(outputCount)
      , m_maxSize(maxSize)
      , m_nodeAdditionProb(nodeAdditionProb)
      , m_nodeFunctionChangeProb(nodeFunctionChangeProb)
      , m_weightChangeProb(weightChangeProb)
      , m_weightInitBound(weightInitBound)
    {
        m_nodes.reserve(maxSize);
        initNet();
    }

    Network::Network(Network const & other)
      : m_inputCount(other.m_inputCount)
      , m_outputCount(other.m_outputCount)
      , m_maxSize(other.m_maxSize)
      , m_nodeAdditionProb(other.m_nodeAdditionProb)
      , m_nodeFunctionChangeProb(other.m_nodeFunctionChangeProb)
      , m_weightChangeProb(other.m_weightChangeProb)
      , m_weightInitBound(other.m_weightInitBound)
      , m_nodes(other.m_nodes)
    {
        // now restore connectivity
        restoreConnectivity(other.m_nodes, 
                            m_nodes, 
                            m_weightInitBound, 
                            m_weightChangeProb);
    }

    Network & Network::operator=(Network const & other)

    {
        if (&other == this) {
            return *this;
        }
        m_inputCount = (other.m_inputCount);
        m_outputCount = (other.m_outputCount);
        m_maxSize = (other.m_maxSize);
        m_nodeAdditionProb = (other.m_nodeAdditionProb);
        m_nodeFunctionChangeProb = (other.m_nodeFunctionChangeProb);
        m_weightChangeProb = (other.m_weightChangeProb);
        m_weightInitBound = (other.m_weightInitBound);
        m_nodes = other.m_nodes;
        // now restore connectivity
        restoreConnectivity(other.m_nodes, 
                            m_nodes, 
                            m_weightInitBound, 
                            m_weightChangeProb);
        return *this;
    }
        

    void Network::initNet()
    {
        // Input and output node creation. 
        for (auto i = 0; i < m_inputCount; ++i) {
            m_nodes.emplace_back(i, NodeType::Input, m_nodeFunctionChangeProb);
            // Also set bias to each input to 1
            m_nodes[i].setExternalInput(1.0);
        }
        for (auto i = m_inputCount; i < m_inputCount + m_outputCount; ++i) {
            m_nodes.emplace_back(i, NodeType::Output, m_nodeFunctionChangeProb);
        }

        // Fully connect from inputs to outputs (i.e. feed-forward)
        for (auto i = 0; i < m_inputCount; ++i) {
            for (auto j = m_inputCount; j < m_inputCount + m_outputCount; ++j) {
                m_nodes[j].addIncomingConnectionFrom(m_nodes[i], 
                                                     m_weightInitBound, 
                                                     m_weightChangeProb);
            }
        }
    }

    void Network::setInput(int const i, double const value) 
    {
        m_nodes[i].setExternalInput(value);
    }

    double Network::getOutput(int const i) const
    {
        auto outputIndex = i + m_inputCount;
        return m_nodes[outputIndex].getOutput();
    }

    void Network::addNodeInPlaceOf(Connection & con)
    {
        // Create a new node but only if number of nodes
        // is less than max permitted size
        auto id = m_nodes.size();
        if (id < m_maxSize) {
            m_nodes.emplace_back(id, NodeType::Hidden, m_nodeFunctionChangeProb);

            // Find out original connectivity
            auto & nodePre = con.getNodeRefA();
            auto & nodePost = con.getNodeRefB();
            
            // Remove old connection from nodePre to nodePost
            auto nodePreIndex = nodePre.getIndex();
            nodePost.removeIncomingConnectionFrom(nodePreIndex);

            // Add new connection from nodePre to new node
            m_nodes[id].addIncomingConnectionFrom(nodePre, 
                                                  m_weightInitBound, 
                                                  m_weightChangeProb);

            // ..and from new node to node post
            nodePost.addIncomingConnectionFrom(m_nodes[id], 
                                               m_weightInitBound, 
                                               m_weightChangeProb);
        }
    }

    void Network::perturbWeights(double const byAmount)
    {
        for(auto i = m_inputCount; i < m_nodes.size(); ++i) {
            m_nodes[i].perturbIncomingWeights(byAmount);
        }
    }
}