/*
  Copyright (c) 2012-2017, Benjamin H.D. Jones
  All rights reserved.
*/


#include "AnimatLayer.hpp"
#include "Vector3.hpp"
#include "PhysicsEngine.hpp"
#include <cmath>
#include <cassert>


namespace model {

    AnimatLayer::AnimatLayer(double const xOffset,
                             double const yOffset,
                             double const width,
                             physics::PhysicsEngine & physicsEngine)
    {
        // instantiate physics
        physics::Vector3 vLeft(xOffset, yOffset, 0);
        physics::Vector3 vRight(xOffset + width, yOffset, 0);
        auto constant = 100;
        auto dampener = 0.9;
        m_indexLeft = physicsEngine.addPointMass(vLeft, 10 /* mass */, false /* not fixed */);
        m_indexRight = physicsEngine.addPointMass(vRight, 10 /* mass */, false /* not fixed */);
        physicsEngine.createSpring(m_indexLeft, m_indexRight, constant, dampener);
    }

    physics::Vector3
    AnimatLayer::getLeftToRightVector(physics::PhysicsEngine const & physicsEngine) const 
    {
        return physicsEngine.getPointMassPosition(m_indexLeft) - 
               physicsEngine.getPointMassPosition(m_indexRight);
    }
    

    physics::Vector3
    AnimatLayer::getRightToLeftVector(physics::PhysicsEngine const & physicsEngine) const 
    {
        return physicsEngine.getPointMassPosition(m_indexRight) - 
               physicsEngine.getPointMassPosition(m_indexLeft);
    }

    
    physics::Vector3
    AnimatLayer::getMidSidePoint(physics::PhysicsEngine const & physicsEngine) const 
    {

        // compute distance between vectors
        physics::Vector3 difference = physicsEngine.getPointMassPosition(m_indexRight) - 
                                      physicsEngine.getPointMassPosition(m_indexLeft);

        // compute halfway distance
        physics::Vector3 halfway = difference / 2;

        // compute mid point as left + halfway
        return physicsEngine.getPointMassPosition(m_indexLeft) + halfway;
    }
    

    int AnimatLayer::getIndexLeft() const 
    {
        return m_indexLeft;
    }

    int AnimatLayer::getIndexRight() const 
    {
        return m_indexRight;
    }

    physics::Vector3
    AnimatLayer::getPositionLeft(physics::PhysicsEngine const & physicsEngine) const
    {
        return physicsEngine.getPointMassPosition(m_indexLeft);
    }

    physics::Vector3
    AnimatLayer::getPositionRight(physics::PhysicsEngine const & physicsEngine) const
    {
        return physicsEngine.getPointMassPosition(m_indexRight);
    }

    physics::Vector3
    AnimatLayer::getVelocityLeft(physics::PhysicsEngine const & physicsEngine) const
    {
        return physicsEngine.getPointMassVelocity(m_indexLeft);
    }

    physics::Vector3
    AnimatLayer::getVelocityRight(physics::PhysicsEngine const & physicsEngine) const
    {
        return physicsEngine.getPointMassVelocity(m_indexLeft);
    }
}
