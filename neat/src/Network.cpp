// Copyright (c) 2017-2020 Ben Jones

#include "neat/Network.hpp"
#include "neat/NodeType.hpp"
#include <cstdlib>
#include <utility>
#include <iostream>
#include <optional>
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

    std::optional<int> containsInnovation(neat::Network::InnovationMap const & map,
                                          int const pre, int const post) {
        auto found = std::find_if(std::begin(map), std::end(map),
                                  [pre, post, &map](std::pair<int, neat::InnovationInfo> const & p) {
                                    return pre == p.second.preNode && post == p.second.postNode;
                                  });
        if(found != std::end(map)) {
            return found->first;
        }
        return std::nullopt;
    }

}

namespace neat {

    int Network::GLOBAL_INNOVATION_NUMBER = 14;
    Network::InnovationMap Network::GLOBAL_INNOVATION_MAP;

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
    
    void Network::assembleInitialInputAndOutputNodes()
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
    }

    void Network::assembleInitialInputToOutputConnectivity()
    {
        // When initializing the network, all connections have the
        // same innovation numbers. It's only when later evolving
        // the connectivity is the number pulled from a global ref
        int innovationNumber = 0;

        // Fully connect from inputs to outputs (i.e. feed-forward)
        for (auto i = 0; i < m_inputCount; ++i) {
            for (auto j = m_inputCount; j < m_inputCount + m_outputCount; ++j) {
                assert(i != j);
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
    }

    void Network::assembleFromInnovationMap()
    {
        // Network is to be assembled from a pre-computed innovation map
        for(auto const & it : m_innovationMap) {
            auto innovInfo = it.second;
            if(innovInfo.enabled) {
                auto preNode = innovInfo.preNode;
                auto postNode = innovInfo.postNode;
                auto weight = innovInfo.weight;
                auto innovationNumber = innovInfo.innovationNumber;

                // Make sure have corect node count. Note that any
                // pre-existing hidden node is automatically mutated here
                // to have a potentially different function.
                if(preNode >= m_nodes.size()) {
                    for (auto i = m_nodes.size(); i <= preNode; ++i) {
                        m_nodes.emplace_back(i, NodeType::Hidden, 
                        m_muts.nodeFunctionChangeProb);
                    }
                }
                if(postNode >= m_nodes.size()) {
                    for (auto i = m_nodes.size(); i <= postNode; ++i) {
                        m_nodes.emplace_back(i, NodeType::Hidden, 
                        m_muts.nodeFunctionChangeProb);
                    }
                }

                m_nodes[postNode].addIncomingConnectionFrom(m_nodes[preNode], 
                                                            m_weightInitBound,
                                                            m_muts.weightChangeProb,
                                                            innovationNumber,
                                                            weight);
            }
        }
    }

    void Network::initNet()
    {
        // Input and output node creation. 
        assembleInitialInputAndOutputNodes();

        if(m_innovationMap.empty()) {
            // Full initial connectivity
            assembleInitialInputToOutputConnectivity();
        } else {
            // Innovation map was constructed during cross over
            assembleFromInnovationMap();
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

    bool Network::addNewNodes()
    {
        // Ouput ID (the node index within the array of nodes)
        // will always start with inputNodeCount + outputNodeCount
        for(int j = m_inputCount ; j < m_nodes.size() ; ++j) {
            auto & node = m_nodes[j];
            for(int i = 0 ; i < m_nodes.size(); ++i) {
                // An output node can't be an input to another node
                if(i >= m_inputCount && i < m_inputCount + m_outputCount) {
                    continue;
                }
                if(node.hasConnectionFrom(i) && i != j) {
                    if (((double) rand() / (RAND_MAX)) < m_muts.nodeAdditionProb) {
                        auto & connection = node.getConnectionFrom(i);
                        return addNodeInPlaceOf(connection);
                    }
                }
            }
        }
        return false;
    }

    bool Network::addNodeInPlaceOf(Connection & con)
    {
        auto added = false;

        // Create a new node but only if number of nodes
        // is less than max permitted size
        auto id = m_nodes.size();
        if (id < m_maxSize) {
            // Find out original connectivity
            auto & nodePre = con.getNodeRefA();
            auto & nodePost = con.getNodeRefB();

            // To prevent loops in the network, prevent any connections
            // between hidden nodes
            if(nodePre.getNodeType() == NodeType::Hidden ||
               nodePost.getNodeType() == NodeType::Hidden) {
                return false;
            }

            m_nodes.emplace_back(id, NodeType::Hidden, 
                     m_muts.nodeFunctionChangeProb);

            // Remove old connection from nodePre to nodePost
            auto nodePreIndex = nodePre.getIndex();
            auto nodePostIndex = nodePost.getIndex();

            {
                auto innovation = nodePost.removeIncomingConnectionFrom(nodePreIndex);
                // Disable the original innovation
                if(innovation > -1) {
                    // m_innovationMap[innovation].enabled = false;
                    // Just erase instead?
                    m_innovationMap.erase(innovation);
                }
            }

            // Do we already have an innovation from nodePre to id?
            {
                auto existsAlready = containsInnovation(GLOBAL_INNOVATION_MAP, nodePreIndex, id);
                int innovationNum = GLOBAL_INNOVATION_NUMBER;
                if(existsAlready) {
                    innovationNum = *existsAlready;
                }

                assert(id != nodePreIndex);

                // Add new connection from nodePre to new node
                m_nodes[id].addIncomingConnectionFrom(nodePre, 
                                                      m_weightInitBound, 
                                                      m_muts.weightChangeProb,
                                                      innovationNum,
                                                      1.0);
                
                // See if we need to add a new global innovation and additionally
                // add to the innovation map of 'this'
                auto innovation = InnovationInfo{innovationNum, 
                                                 nodePre.getIndex(), 
                                                 static_cast<int>(id),
                                                 1.0 /* weight */, 
                                                 true};
                if(!existsAlready) {      
                    GLOBAL_INNOVATION_MAP.emplace(GLOBAL_INNOVATION_NUMBER, innovation);
                    ++GLOBAL_INNOVATION_NUMBER;
                } else {
                    added = true;
                }
                m_innovationMap.emplace(innovationNum, innovation);
            }
            
            // Do we already have an innovation from id to nodePost?
            {
                auto existsAlready = containsInnovation(GLOBAL_INNOVATION_MAP, id, nodePostIndex);
                int innovationNum = GLOBAL_INNOVATION_NUMBER;
                if(existsAlready) {
                    innovationNum = *existsAlready;
                }
                assert(id != nodePostIndex);
                nodePost.addIncomingConnectionFrom(m_nodes[id], 
                                                   m_weightInitBound, 
                                                   m_muts.weightChangeProb,
                                                   innovationNum,
                                                   con.weight());
                auto innovation = InnovationInfo{innovationNum, 
                                                 static_cast<int>(id),
                                                 nodePost.getIndex(),
                                                 con.weight(), 
                                                 true};
                if(!existsAlready) {      
                    GLOBAL_INNOVATION_MAP.emplace(GLOBAL_INNOVATION_NUMBER, innovation);
                    ++GLOBAL_INNOVATION_NUMBER;
                } else {
                    added = true;
                }
                m_innovationMap.emplace(innovationNum, innovation);
            }
        }
        return added;
    }

    void Network::perturbWeights(double const byAmount)
    {
        for(auto i = m_inputCount; i < m_nodes.size(); ++i) {
            m_nodes[i].perturbIncomingWeights(byAmount);
        }
    }

    bool Network::addConnectionToHiddenOrOutputNode()
    {
        auto it = std::begin(m_nodes) + m_inputCount;
        if (it != std::end(m_nodes)) {
            for(; it != std::end(m_nodes); ++it) {
                for (int i = 0; i < m_inputCount; ++i) {
                    if(!it->hasConnectionFrom(i)) {
                        if (((double) rand() / (RAND_MAX)) < m_muts.connectionAdditionProb) {
                            // Do we already have an innovation from id to nodePost?
                            {
                                auto existsAlready = containsInnovation(GLOBAL_INNOVATION_MAP, i, it->getIndex());
                                int innovationNum = GLOBAL_INNOVATION_NUMBER;
                                if(existsAlready) {
                                    innovationNum = *existsAlready;
                                }

                                it->addIncomingConnectionFrom(m_nodes[i], 
                                                              m_weightInitBound,
                                                              m_muts.weightChangeProb,
                                                              innovationNum);
                                auto const weight = it->getConnectionWeightFrom(i);
                                auto innovation = InnovationInfo{innovationNum, 
                                                                 i,
                                                                 it->getIndex(),
                                                                 weight, 
                                                                 true};
                                if(!existsAlready) {      
                                    GLOBAL_INNOVATION_MAP.emplace(GLOBAL_INNOVATION_NUMBER, innovation);
                                    ++GLOBAL_INNOVATION_NUMBER;
                                    return false;
                                } else {
                                    return true;
                                }
                                m_innovationMap.emplace(innovationNum, innovation);
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    void Network::perturbNodeFunctions()
    {
        for (auto & node : m_nodes) {
            node.perturbNodeFunction();
        }
    }

    bool Network::mutate()
    {

        // Return true if a mutation led to a new species
        auto newCon = false;
        auto newNode = false;
        perturbWeights(m_weightInitBound / 4.0);
        auto random_integer = uni(rng);
        
        if(random_integer == 0) {
            newCon = addConnectionToHiddenOrOutputNode();
        } else if(random_integer == 1) {
            perturbNodeFunctions();
        } else {
            newNode = addNewNodes();
        }
        return newCon || newNode;
    }

    Network Network::crossWith(Network const & other) const
    {
        InnovationMap crossedMap;

        // Iterate through map and find matching genes. A matching gene
        // is chosen at random from either parent
        for (auto const & innovation : m_innovationMap) {
            // See if this innovation exists in the other map
            auto found = other.m_innovationMap.find(innovation.second.innovationNumber);
            if (found != std::end(other.m_innovationMap)) {
                if (((double) rand() / (RAND_MAX)) < 0.5) {
                    crossedMap.emplace(innovation.second.innovationNumber, innovation.second);
                } else {
                    crossedMap.emplace(innovation.second.innovationNumber, found->second);
                }
            }
        }

        // Genes that are excess or disjoint come from the fittest. The
        // fittest is always the other network. This should have been
        // taken into account when calling the crossWith function.
        for (auto const & innovation : other.m_innovationMap) {
            auto found = m_innovationMap.find(innovation.second.innovationNumber);
            // If not found, means it is disjoint and should be added to crossed map
            if(found == std::end(m_innovationMap)) {
                crossedMap.emplace(innovation.second.innovationNumber, innovation.second);
            }
        }
        return Network(m_inputCount,
                       m_outputCount,
                       m_maxSize,
                       m_muts,
                       m_weightInitBound,
                       crossedMap);
    }

    double Network::measureDifference(Network const & other) const
    {
        auto difference = 0.0;

        // for (auto const & innovation : m_innovationMap) {
        //     // See if this innovation exists in the other map
        //     auto found = other.m_innovationMap.find(innovation.second.innovationNumber);
        //     if (found != std::end(other.m_innovationMap)) {
        //         auto const w1 = innovation.second.weight;
        //         auto const w2 = found->second.weight;
        //         if(w1 >= w2) {
        //             difference += (w1 - w2);
        //         } else {
        //             difference += (w2 - w1);
        //         }
        //     }
        // }

        for (auto const & innovation : other.m_innovationMap) {
            auto found = m_innovationMap.find(innovation.second.innovationNumber);
            // If not found, means it is disjoint and should be added to crossed map
            if(found == std::end(m_innovationMap)) {
                difference += 1.0;
            }
        }
        for (auto const & innovation : m_innovationMap) {
            auto found = other.m_innovationMap.find(innovation.second.innovationNumber);
            // If not found, means it is disjoint and should be added to crossed map
            if(found == std::end(other.m_innovationMap)) {
                difference += 1.0;
            }
        }
        return difference;
    }
}