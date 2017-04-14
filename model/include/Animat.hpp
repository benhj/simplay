/// Copyright (c) 2017 Ben Jones
#pragma once

#include "PhysicsEngine.hpp"
#include "AnimatLayer.hpp"
#include "AnimatBlock.hpp"
#include "Vector3.hpp"
#include <vector>
#include <mutex>

namespace model {

    class Animat
    {

      public:
        Animat(int const layers,
               double const layerWidth,
               double const blockHeight);
   
        void applyBlockContraction(int const block, 
                                   int const side, 
                                   double const force);

        void relaxBlockContraction(int const block, 
                                   int const side);

        void applyWaterForces();

        /// Integrates animat physics
        void update();

        AnimatBlock const & getBlock(int const b) const;

        physics::PhysicsEngine const & getPhysicsEngine() const;

        physics::Vector3 getLeftAntennaePoint() const;
        physics::Vector3 getRightAntennaePoint() const;

      private:
        std::vector<AnimatLayer> m_layers;
        std::vector<AnimatBlock> m_blocks;
        physics::PhysicsEngine m_physicsEngine;
        physics::Vector3 m_leftAntenna;
        physics::Vector3 m_rightAntenna;
        mutable std::mutex m_antennaeMutex;
        void constructAntennae();
    };
}

