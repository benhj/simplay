#include "AnimatBlock.hpp"

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace model {

    AnimatBlock::AnimatBlock(AnimatLayer const & layerOne,
                             AnimatLayer const & layerTwo,
                             physics::PhysicsEngine & physicsEngine)
      : m_layerOne(layerOne)
      , m_layerTwo(layerTwo)
    {
        auto const layerOneLeftVertIndex = m_layerOne.getIndexLeft();
        auto const layerOneRightVertIndex = m_layerOne.getIndexRight();
        auto const layerTwoLeftVertIndex = m_layerTwo.getIndexLeft();
        auto const layerTwoRightVertIndex = m_layerTwo.getIndexRight();

        // create springs between point masses ('X')
        physicsEngine.createSpring(layerOneLeftVertIndex,
                                   layerTwoLeftVertIndex,
                                   1.0 /* constant */, 
                                   0.9 /* dampener */);

        physicsEngine.createSpring(layerOneRightVertIndex,
                                   layerTwoRightVertIndex,
                                   1.0 /* constant */, 
                                   0.9 /* dampener */);

        physicsEngine.createSpring(layerOneLeftVertIndex,
                                   layerTwoRightVertIndex,
                                   1.0 /* constant */, 
                                   0.9 /* dampener */);

        physicsEngine.createSpring(layerOneRightVertIndex,
                                   layerTwoLeftVertIndex,
                                   1.0 /* constant */, 
                                   0.9 /* dampener */);

    }

    AnimatLayer const & AnimatBlock::getLayerOne() const {return m_layerOne;}
    AnimatLayer const & AnimatBlock::getLayerTwo() const {return m_layerTwo;}
}
