/// Copyright (c) 2017 Ben Jones

#include "AnimatWorld.hpp"

namespace simulator {
    AnimatWorld::AnimatWorld(int const populationSize,
                             model::AnimatProperties const & animatProperties)
      : m_animats()
    {
        m_animats.reserve(populationSize);
        m_animats.emplace_back(animatProperties.blocks + 1,
                               animatProperties.blockWidth,
                               animatProperties.blockHeight);
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
}