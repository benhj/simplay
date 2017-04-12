/*
  Copyright (c) 2012-2017, Benjamin H.D. Jones
  All rights reserved.
*/

#pragma once

/**
 *      Stores vertex point information for a given layer of the animat. These layers
 *      are then connected together to form body segments
 *
 *      Ben Jones 2010
 *
 */

#include "Vector3.hpp"
#include "PhysicsEngine.hpp"
#include <memory>

namespace model {

    class AnimatLayer
    {

      public:
        AnimatLayer(double const xOffset,
                    double const yOffset,
                    double const width,
                    physics::PhysicsEngine & physicsEngine);

        AnimatLayer() = delete;

        int getIndexLeft() const;
        int getIndexRight() const;

        physics::Vector3 getLeftToRightVector(physics::PhysicsEngine const & physicsEngine) const;
        physics::Vector3 getRightToLeftVector(physics::PhysicsEngine const & physicsEngine) const;
        physics::Vector3 getMidSidePoint(physics::PhysicsEngine const & physicsEngine) const;
        physics::Vector3 const & getPositionLeft(physics::PhysicsEngine const & physicsEngine) const;
        physics::Vector3 const & getPositionRight(physics::PhysicsEngine const & physicsEngine) const;

     private:
       int m_indexLeft;
       int m_indexRight;
    }; 
}


