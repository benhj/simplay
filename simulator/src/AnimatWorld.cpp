/// Copyright (c) 2017 Ben Jones

#include "AnimatWorld.hpp"
#include <cstdlib> // rand
#include <ctime> // time

namespace simulator {
    AnimatWorld::AnimatWorld(int const populationSize,
                             model::AnimatProperties const & animatProperties)
      : m_animats()
    {
        m_animats.reserve(populationSize);
        m_animats.emplace_back(animatProperties.blocks + 1,
                               animatProperties.blockWidth,
                               animatProperties.blockHeight);

        // seed random generator for random pop placement
        ::srand(::time(NULL));
    }

    void AnimatWorld::randomizePositions(double const boundX,
                                         double const boundY)
    {
        for (auto i = 0; i < m_animats.size(); ++i) {
            auto randomX = ((double) rand() / (RAND_MAX)) * boundX;
            auto randomY = ((double) rand() / (RAND_MAX)) * boundY;
            auto posOrNeg = ((double) rand() / (RAND_MAX));
            if (posOrNeg >= 0.5) {
                randomX = -randomX;
            }
            posOrNeg = ((double) rand() / (RAND_MAX));
            if (posOrNeg >= 0.5) {
                randomY = -randomY;
            }
            translateAnimatPosition(i, randomX, randomY);
        }
    }

    void AnimatWorld::update()
    {
        for (auto & animat : m_animats) {
            animat.update();
        }
    }

    model::Animat & AnimatWorld::animat(int const index)
    {
        return m_animats[index];
    }

    void AnimatWorld::translateAnimatPosition(int const index,
                                              double const x, 
                                              double const y)
    {
        auto & animat = m_animats[index];
        auto & physicsEngine = animat.getPhysicsEngine();
        auto const blockCount = animat.getBlockCount();

        // First update the point mass positions
        for (auto lay = 0 ; lay <= blockCount; ++lay) {
            auto & layer = animat.getLayer(lay);
            auto const indexLeft = layer.getIndexLeft();
            auto const indexRight = layer.getIndexRight();
            auto & leftPos = physicsEngine.getPointMassPositionRef(indexLeft);
            auto & rightPos = physicsEngine.getPointMassPositionRef(indexRight);
            leftPos.m_vec[0] += x;
            leftPos.m_vec[1] += y;
            rightPos.m_vec[0] += x;
            rightPos.m_vec[1] += y;
        }

        // Then update the derived stuff (antenna, bounding circles etc.)
        animat.updateDerivedComponents();
    }
}