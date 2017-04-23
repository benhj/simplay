/// Copyright (c) 2017 Ben Jones
#pragma once

/**
 * An animat block is a class storing two layers of the animat. The blocks
 * fit together to form the whole animat
 *
 * Ben Jones 2010
 *
 */

#include "AnimatLayer.hpp"
#include "PhysicsEngine.hpp"
#include "Vector3.hpp"

namespace model {

    class AnimatBlock
    {
      public:
        
        AnimatBlock(AnimatLayer & layerOne,
                    AnimatLayer & layerTwo,
                    double const constant,
                    physics::PhysicsEngine & physicsEngine);
        AnimatBlock() = delete;

        /**
         * Access the animat layers making up the segment
         */
        AnimatLayer & getLayerOne();
        AnimatLayer & getLayerTwo();

        void contractLeftSide(physics::PhysicsEngine & physicsEngine,
                              double const forceMagnitude);
        void contractRightSide(physics::PhysicsEngine & physicsEngine,
                              double const forceMagnitude);

        void relaxLeftSide(physics::PhysicsEngine & physicsEngine);
        void relaxRightSide(physics::PhysicsEngine & physicsEngine);

        /// Draws a bounding circle around block. Will also be useful
        /// for collision dectection and resolution process.
        std::pair<physics::Vector3, double> 
        deriveBoundingCircle(physics::PhysicsEngine & physicsEngine);

      private:

        AnimatLayer & m_layerOne;
        AnimatLayer & m_layerTwo;
        int m_leftSpringIndex;
        int m_rightSpringIndex;
    };

}

