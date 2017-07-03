// Copyright (c) 2017 Ben Jones

#include "neat/Network.hpp"
#include "neat/NodeType.hpp"
#include "neat/GlobalInnovationNumber.hpp"
#include <cstdlib>
#include <utility>

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
                    auto const weight = oldNodes[j].getConnectionWeightFrom(i);
                    auto const innovNumber = oldNodes[j].getInnovNumberForConnectionFrom(i);
                    newNodes[j].addIncomingConnectionFrom(newNodes[i], 
                                                          weightInitBound,
                                                          weightChangeProb,
                                                          innovNumber,
                                                          weight);
                }
            }
        }
    }
}

namespace neat {
    Network::Network(int const inputCount, 
                     int const outputCount,
                     int const maxSize,
                     MutationParameters const & muts,
                     double const weightInitBound)
      : m_inputCount(inputCount)
      , m_outputCount(outputCount)
      , m_maxSize(maxSize)
      , m_muts(muts)
      , m_weightInitBound(weightInitBound)
      , m_innovationMap()
    {
        m_nodes.reserve(maxSize);
        m_outputIDs.reserve(outputCount);
        initNet();
    }

    Network::Network(Network const & other)
      : m_inputCount(other.m_inputCount)
      , m_outputCount(other.m_outputCount)
      , m_maxSize(other.m_maxSize)
      , m_muts(other.m_muts)
      , m_weightInitBound(other.m_weightInitBound)
      , m_nodes(other.m_nodes)
      , m_outputIDs(other.m_outputIDs)
      , m_innovationMap(other.m_innovationMap)
    {
        // now restore connectivity
        restoreConnectivity(other.m_nodes, 
                            m_nodes, 
                            m_weightInitBound, 
                            m_muts.weightChangeProb);
    }

    Network & Network::operator=(Network const & other)

    {
        if (&other == this) {
            return *this;
        }
        m_inputCount = other.m_inputCount;
        m_outputCount = other.m_outputCount;
        m_maxSize = other.m_maxSize;
        m_muts = other.m_muts;
        m_weightInitBound = other.m_weightInitBound;
        m_nodes = other.m_nodes;
        m_innovationMap = other.m_innovationMap;
        // now restore connectivity
        restoreConnectivity(other.m_nodes, 
                            m_nodes, 
                            m_weightInitBound, 
                            m_muts.weightChangeProb);
        return *this;
    }
        

    void Network::initNet()
    {
        // Input and output node creation. 
        for (auto i = 0; i < m_inputCount; ++i) {
            m_nodes.emplace_back(i, NodeType::Input, 
                                 m_muts.nodeFunctionChangeProb);
            // Also set bias to each input to 1
            m_nodes[i].setExternalInput(1.0);
        }
        for (auto i = m_inputCount; i < m_inputCount + m_outputCount; ++i) {
            m_nodes.emplace_back(i, NodeType::Output, 
                                 m_muts.nodeFunctionChangeProb);
            m_outputIDs.push_back(i);
        }

        // When initializing the network, all connections have the
        // same innovation numbers. It's only when later evolving
        // the connectivity it the number pulled from a global ref
        int innovationNumber = 0;

        // Fully connect from inputs to outputs (i.e. feed-forward)
        for (auto i = 0; i < m_inputCount; ++i) {
            for (auto j = m_inputCount; j < m_inputCount + m_outputCount; ++j) {
                m_nodes[j].addIncomingConnectionFrom(m_nodes[i], 
                                                     m_weightInitBound, 
                                                     m_muts.weightChangeProb,
                                                     innovationNumber);
                m_innovationMap.emplace(innovationNumber,
                                        InnovationInfo{innovationNumber, i, j, true});
                ++innovationNumber;
            }
        }
    }

    void Network::setInput(int const i, double const value) 
    {
        m_nodes[i].setExternalInput(value);
    }

    double Network::getOutput(int const i) const
    {
        auto outputIndex = m_outputIDs[i];
        return m_nodes[outputIndex].getOutput();
    }

    void Network::addNewNodes()
    {
        // Ouput ID (the node index within the array of nodes)
        // will always be inputNodeCount + outputNodeCount
        for(int j = 0 ; j < m_outputCount ; ++j) {
            auto const outputID = m_inputCount + j;
            auto & node = m_nodes[outputID];
            for(int i = 0 ; i < m_nodes.size(); ++i) {
                if(i == outputID) {
                    continue;
                }
                if(node.hasConnectionFrom(i)) {
                    if (((double) rand() / (RAND_MAX)) < m_muts.nodeAdditionProb) {
                        auto & connection = node.getConnectionFrom(i);
                        addNodeInPlaceOf(connection);
                    }
                }
            }
        }
        
    }

    void Network::addNodeInPlaceOf(Connection & con)
    {
        // Create a new node but only if number of nodes
        // is less than max permitted size
        auto id = m_nodes.size();
        if (id < m_maxSize) {
            m_nodes.emplace_back(id, NodeType::Hidden, 
                                 m_muts.nodeFunctionChangeProb);

            // Find out original connectivity
            auto & nodePre = con.getNodeRefA();
            auto & nodePost = con.getNodeRefB();
            
            // Remove old connection from nodePre to nodePost
            auto nodePreIndex = nodePre.getIndex();
            nodePost.removeIncomingConnectionFrom(nodePreIndex);

            // Add new connection from nodePre to new node
            m_nodes[id].addIncomingConnectionFrom(nodePre, 
                                                  m_weightInitBound, 
                                                  m_muts.weightChangeProb,
                                                  GLOBAL_INNOVATION_NUMBER);

            m_innovationMap.emplace(GLOBAL_INNOVATION_NUMBER,
                                    InnovationInfo{GLOBAL_INNOVATION_NUMBER, 
                                                   nodePre.getIndex(), 
                                                   static_cast<int>(id), 
                                                   true});

            ++GLOBAL_INNOVATION_NUMBER;

            // ..and from new node to node post
            nodePost.addIncomingConnectionFrom(m_nodes[id], 
                                               m_weightInitBound, 
                                               m_muts.weightChangeProb,
                                               GLOBAL_INNOVATION_NUMBER);

            m_innovationMap.emplace(GLOBAL_INNOVATION_NUMBER,
                                    InnovationInfo{GLOBAL_INNOVATION_NUMBER, 
                                                   static_cast<int>(id),
                                                   nodePost.getIndex(), 
                                                   true});

            ++GLOBAL_INNOVATION_NUMBER;
        }
    }

    void Network::perturbWeights(double const byAmount)
    {
        for(auto i = m_inputCount; i < m_nodes.size(); ++i) {
            m_nodes[i].perturbIncomingWeights(byAmount);
        }
    }

    void Network::addConnectionToHiddenNode()
    {
        auto it = std::begin(m_nodes) + m_inputCount + m_outputCount;
        if (it != std::end(m_nodes)) {
            for(; it != std::end(m_nodes); ++it) {
                for (int i = 0; i < m_inputCount; ++i) {
                    if(!it->hasConnectionFrom(i)) {
                        if (((double) rand() / (RAND_MAX)) < m_muts.connectionAdditionProb) {
                            it->addIncomingConnectionFrom(m_nodes[i], 
                                                          m_weightInitBound,
                                                          m_muts.weightChangeProb,
                                                          GLOBAL_INNOVATION_NUMBER++);
                        }
                    }
                }
            }
        }

    }

    void Network::mutate()
    {
        perturbWeights(m_weightInitBound / 4.0);
        addConnectionToHiddenNode();
        addNewNodes();
    }
}