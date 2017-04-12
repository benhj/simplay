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

namespace model {

    class AnimatBlock
    {
      public:
        
        AnimatBlock(AnimatLayer const & layerOne,
                    AnimatLayer const & layerTwo,
                    physics::PhysicsEngine & physicsEngine);
        AnimatBlock() = delete;

        /**
         * Access the animat layers making up the segment
         */
        AnimatLayer const & getLayerOne() const;
        AnimatLayer const & getLayerTwo() const;

        void contractLeftSide(physics::PhysicsEngine & physicsEngine,
                              double const forceMagnitude);
        void contractRightSide(physics::PhysicsEngine & physicsEngine,
                              double const forceMagnitude);

      private:

        AnimatLayer const & m_layerOne;
        AnimatLayer const & m_layerTwo;
        int m_leftSpringIndex;
        int m_rightSpringIndex;
    };

}

