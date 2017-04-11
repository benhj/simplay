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
      : m_xOffset(xOffset)
      , m_yOffset(yOffset)
      , m_width(width)
      , m_leftPoint(xOffset, yOffset, 0)
      , m_rightPoint(xOffset + width, yOffset, 0)
    {
        // instantiate physics
        m_indexLeft = physicsEngine.addPointMass(m_leftPoint, 5.0 /* mass */, false /* not fixed */);
        m_indexRight = physicsEngine.addPointMass(m_rightPoint, 5.0 /* mass */, false /* not fixed */);
        physicsEngine.createSpring(m_indexLeft, m_indexRight, 1.0 /* constant */, 0.9 /* dampener */);
    }

    physics::Vector3
    AnimatLayer::getLeftToRightVector() const
    {
        return m_rightPoint - m_leftPoint;
    }

    physics::Vector3
    AnimatLayer::getRightToLeftVector() const
    {
        return m_leftPoint - m_rightPoint;
    }


    physics::Vector3
    AnimatLayer::getMidSidePoint() const
    {

        // compute distance between vectors
        physics::Vector3 difference = m_rightPoint - m_leftPoint;

        // compute halfway distance
        physics::Vector3 halfway = difference / 2;

        // compute mid point as left + halfway
        return m_leftPoint + halfway;

    }

    physics::Vector3 const & AnimatLayer::getLeftPoint() const
    {
        return m_leftPoint;
    }

    physics::Vector3 const & AnimatLayer::getRightPoint() const
    {
        return m_rightPoint;
    }

    void AnimatLayer::updateLeftPoint(physics::PhysicsEngine const & physicsEngine) 
    {
        //m_leftPoint = physicsEngine.getMassPointPosition(m_indexLeft);
    }
    void AnimatLayer::updateRightPoint(physics::PhysicsEngine const & physicsEngine) 
    {
        //m_rightPoint = physicsEngine.getMassPointPosition(m_indexRight);
    }
}
