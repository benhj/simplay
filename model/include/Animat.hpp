#pragma once

#include "PhysicsEngine.hpp"
#include "AnimatLayer.hpp"
#include "AnimatBlock.hpp"
#include <vector>

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
        void update();

      private:
        std::vector<AnimatLayer> m_layers;
        std::vector<AnimatBlock> m_blocks;
        physics::PhysicsEngine m_physicsEngine;
    };
}

