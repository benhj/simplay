// Copyright (c) 2017 Ben Jones

#include "neat/Network.hpp"
#include "neat/NodeType.hpp"
#include <cstdlib>
#include <utility>
#include <iostream>

#include <random>

namespace {

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0,2); // guaranteed unbiased

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

    int Network::GLOBAL_INNOVATION_NUMBER = 5;

    Network::Network(int const inputCount, 
                     int const outputCount,
                     int const maxSize,
                     MutationParameters const & muts,
                     double const weightInitBound,
                     InnovationMap const & innovMap)
      : m_inputCount(inputCount)
      , m_outputCount(outputCount)
      , m_maxSize(maxSize)
      , m_muts(muts)
      , m_weightInitBound(weightInitBound)
      , m_innovationMap(innovMap)
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
        }
        for (auto i = m_inputCount; i < m_inputCount + m_outputCount; ++i) {
            m_nodes.emplace_back(i, NodeType::Output, 
                                 m_muts.nodeFunctionChangeProb);
            m_outputIDs.push_back(i);
        }

        // // Also SEED with a single hidden node
        // m_nodes.emplace_back(m_inputCount + m_outputCount, 
        //                      NodeType::Hidden, 
        //                      m_muts.nodeFunctionChangeProb);

        if(m_innovationMap.empty()) {
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
                    auto const weight = m_nodes[j].getConnectionWeightFrom(i);
                    m_innovationMap.emplace(innovationNumber,
                                            InnovationInfo{innovationNumber, i, j, weight, true});
                    ++innovationNumber;
                }
            }

            // // Fully connect from inputs to hidden
            // for (auto i = 0; i < m_inputCount; ++i) {
            //     auto const hiddenID = m_inputCount + m_outputCount;
            //     m_nodes[hiddenID].addIncomingConnectionFrom(m_nodes[i], 
            //                                                 m_weightInitBound, 
            //                                                 m_muts.weightChangeProb,
            //                                                 innovationNumber);

            //     auto const weight = m_nodes[hiddenID].getConnectionWeightFrom(i);

            //     m_innovationMap.emplace(innovationNumber,
            //                             InnovationInfo{innovationNumber, i, hiddenID, weight, true});
            //     ++innovationNumber;
            // }

            // // Fully connect from hidden to output
            // auto const hiddenID = m_inputCount + m_outputCount;
            // auto const outputID = hiddenID - 1;
            // m_nodes[outputID].addIncomingConnectionFrom(m_nodes[hiddenID], 
            //                                             m_weightInitBound, 
            //                                             m_muts.weightChangeProb,
            //                                             innovationNumber);
            // auto const weight = m_nodes[outputID].getConnectionWeightFrom(hiddenID);
            // m_innovationMap.emplace(innovationNumber,
            //                         InnovationInfo{innovationNumber, hiddenID, outputID, weight, true});
            // ++innovationNumber;

        } else {

            // Network is to be assembled from a pre-computed innovation map
            for(auto const & it : m_innovationMap) {
                auto & innovInfo = it.second;
                if(innovInfo.enabled) {
                    auto & preNode = innovInfo.preNode;
                    auto & postNode = innovInfo.postNode;
                    auto & weight = innovInfo.weight;
                    auto & innovationNumber = innovInfo.innovationNumber;

                    m_nodes[postNode].addIncomingConnectionFrom(m_nodes[preNode], 
                                                                m_weightInitBound,
                                                                m_muts.weightChangeProb,
                                                                innovationNumber,
                                                                weight);
                }
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
            auto innovation = nodePost.removeIncomingConnectionFrom(nodePreIndex);

            // Disable the original innovation
            if(innovation > -1) {
                m_innovationMap[innovation].enabled = false;
            }

            // Add new connection from nodePre to new node
            m_nodes[id].addIncomingConnectionFrom(nodePre, 
                                                  m_weightInitBound, 
                                                  m_muts.weightChangeProb,
                                                  GLOBAL_INNOVATION_NUMBER);

            auto weight = m_nodes[id].getConnectionWeightFrom(nodePre.getIndex());

            m_innovationMap.emplace(GLOBAL_INNOVATION_NUMBER,
                                    InnovationInfo{GLOBAL_INNOVATION_NUMBER, 
                                                   nodePre.getIndex(), 
                                                   static_cast<int>(id),
                                                   weight, 
                                                   true});

            ++GLOBAL_INNOVATION_NUMBER;

            // ..and from new node to node post
            nodePost.addIncomingConnectionFrom(m_nodes[id], 
                                               m_weightInitBound, 
                                               m_muts.weightChangeProb,
                                               GLOBAL_INNOVATION_NUMBER);

            weight = m_nodes[nodePost.getIndex()].getConnectionWeightFrom(id);

            m_innovationMap.emplace(GLOBAL_INNOVATION_NUMBER,
                                    InnovationInfo{GLOBAL_INNOVATION_NUMBER, 
                                                   static_cast<int>(id),
                                                   nodePost.getIndex(),
                                                   weight, 
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
                                                          GLOBAL_INNOVATION_NUMBER);
                            auto const weight = it->getConnectionWeightFrom(i);
                            m_innovationMap.emplace(GLOBAL_INNOVATION_NUMBER,
                                                    InnovationInfo{GLOBAL_INNOVATION_NUMBER, 
                                                                   i,
                                                                   it->getIndex(),
                                                                   weight,
                                                                   true});
                            ++GLOBAL_INNOVATION_NUMBER;
                        }
                    }
                }
            }
        }

    }

    void Network::perturbNodeFunctions()
    {
        for (auto & node : m_nodes) {
            node.perturbNodeFunction();
        }
    }

    void Network::mutate()
    {
        perturbWeights(m_weightInitBound / 4.0);

        auto random_integer = uni(rng);

        if(random_integer == 0) {
            addConnectionToHiddenNode();
        } else if(random_integer == 1) {
            perturbNodeFunctions();
        } else {
            addNewNodes();
        }
    }

    Network Network::crossWith(Network const & other) const
    {
        InnovationMap crossedMap = m_innovationMap;

        // Iterate through other map, inserting all genes
        // that are not in crossed map
        for(auto const & innovation : other.m_innovationMap) {
            auto innovationID = innovation.first;
            if (crossedMap.find(innovationID) == std::end(crossedMap)) {
                auto theInnovation = innovation.second;
                crossedMap.emplace(innovationID, std::move(theInnovation));
            }
        }
        return Network(m_inputCount,
                       m_outputCount,
                       m_maxSize,
                       m_muts,
                       m_weightInitBound,
                       crossedMap);
    }
}