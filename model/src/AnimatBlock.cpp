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

        auto constant = 25;
        auto dampener = 0.4;

        // create springs between point masses ('X')
        m_leftSpringIndex =
        physicsEngine.createSpring(layerOneLeftVertIndex,
                                   layerTwoLeftVertIndex,
                                   constant,
                                   dampener);

        m_rightSpringIndex =
        physicsEngine.createSpring(layerOneRightVertIndex,
                                   layerTwoRightVertIndex,
                                   constant,
                                   dampener);
        (void)
        physicsEngine.createSpring(layerOneLeftVertIndex,
                                   layerTwoRightVertIndex,
                                   50,
                                   dampener);

        (void)
        physicsEngine.createSpring(layerOneRightVertIndex,
                                   layerTwoLeftVertIndex,
                                   50,
                                   dampener);
    }

    AnimatLayer const & AnimatBlock::getLayerOne() const {return m_layerOne;}
    AnimatLayer const & AnimatBlock::getLayerTwo() const {return m_layerTwo;}

    void AnimatBlock::contractLeftSide(physics::PhysicsEngine & physicsEngine,
                                       double const forceMagnitude)
    {
        physicsEngine.compressSpring(m_leftSpringIndex, forceMagnitude);
    }

    void AnimatBlock::contractRightSide(physics::PhysicsEngine & physicsEngine,
                                       double const forceMagnitude)
    {
        physicsEngine.compressSpring(m_rightSpringIndex, forceMagnitude);
    }

    void AnimatBlock::relaxLeftSide(physics::PhysicsEngine & physicsEngine)
    {
        physicsEngine.relaxSpring(m_leftSpringIndex);
    }

    void AnimatBlock::relaxRightSide(physics::PhysicsEngine & physicsEngine)
    {
        physicsEngine.relaxSpring(m_rightSpringIndex);
    }
}
