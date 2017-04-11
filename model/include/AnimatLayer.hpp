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

        physics::Vector3 getLeftToRightVector() const;
        physics::Vector3 getRightToLeftVector() const;
        physics::Vector3 getMidSidePoint() const;
        physics::Vector3 const & getLeftPoint() const;
        physics::Vector3 const & getRightPoint() const;

        /// Update vector positions according to current
        /// state of the physics engine
        void updateLeftPoint(physics::PhysicsEngine const & pe);
        void updateRightPoint(physics::PhysicsEngine const & pe);

     private:
       double m_xOffset;
       double m_yOffset;
       double m_width;
       physics::Vector3 m_leftPoint;
       physics::Vector3 m_rightPoint;
       int m_indexLeft;
       int m_indexRight;
    }; 
}


