/// Copyright (c) 2017 Ben Jones

#include "model/AnimatBlock.hpp"

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace model {

    AnimatBlock::AnimatBlock(AnimatLayer & layerOne,
                             AnimatLayer & layerTwo,
                             double const constant,
                             physics::PhysicsEngine & physicsEngine)
      : m_layerOne(layerOne)
      , m_layerTwo(layerTwo)
    {
        auto const layerOneLeftVertIndex = m_layerOne.getIndexLeft();
        auto const layerOneRightVertIndex = m_layerOne.getIndexRight();
        auto const layerTwoLeftVertIndex = m_layerTwo.getIndexLeft();
        auto const layerTwoRightVertIndex = m_layerTwo.getIndexRight();

        auto constantB = 300;
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
                                   constantB,
                                   dampener);

        (void)
        physicsEngine.createSpring(layerOneRightVertIndex,
                                   layerTwoLeftVertIndex,
                                   constantB,
                                   dampener);
    }

    AnimatLayer & AnimatBlock::getLayerOne() {return m_layerOne;}
    AnimatLayer & AnimatBlock::getLayerTwo() {return m_layerTwo;}

    void AnimatBlock::contractLeftSide(physics::PhysicsEngine & physicsEngine,
                                       double const forceMagnitude)
    {
        //physicsEngine.updateSpringConstant(m_leftSpringIndex, 30.0 - forceMagnitude);
        //physicsEngine.updateSpringConstant(m_rightSpringIndex, 20.0 + forceMagnitude);
        physicsEngine.compressSpring(m_leftSpringIndex, forceMagnitude);
    }

    void AnimatBlock::contractRightSide(physics::PhysicsEngine & physicsEngine,
                                       double const forceMagnitude)
    {
        //physicsEngine.updateSpringConstant(m_leftSpringIndex, 30.0 + forceMagnitude);
        //physicsEngine.updateSpringConstant(m_rightSpringIndex, 30.0 - forceMagnitude);
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

    std::pair<physics::Vector3, double> 
    AnimatBlock::deriveBoundingCircle(physics::PhysicsEngine & physicsEngine)
    {
        auto layerOneLeft = m_layerOne.getPositionLeft(physicsEngine);
        auto layerOneRight = m_layerOne.getPositionRight(physicsEngine);
        auto layerTwoLeft = m_layerTwo.getPositionLeft(physicsEngine);
        auto layerTwoRight = m_layerTwo.getPositionRight(physicsEngine);

        // Construct mid-point
        auto firstApprox = ((layerTwoLeft - layerOneLeft) / 2) + layerOneLeft;
        auto secondApprox = ((layerTwoRight - layerOneRight) / 2) + layerOneRight;

        // Construct radius
        auto rightLeftDist = layerTwoRight.distance(layerOneLeft);
        auto leftRightDist = layerTwoLeft.distance(layerOneRight);
        auto radius = (rightLeftDist + leftRightDist) / 4.0;
        
        return {(firstApprox + secondApprox) / 2, radius};   
    }
}
