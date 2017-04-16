/// Copyright (c) 2017 Ben Jones

#pragma once

/// For describing a population of animats in a given world.
/// Responsible for initializing and updating the world.

#include "Animat.hpp"
#include "AnimatProperties.hpp"
#include <vector>

namespace simulator {
    class AnimatWorld
    {
       public:
         AnimatWorld(int const populationSize,
                     model::AnimatProperties const & animatProperties);
         AnimatWorld() = delete;

         /// Randomizes individual placements with bounds
         /// that specify how big the environment is. Note
         /// ranges are [-boundX, boundX] and [-boundY, boundY]
         void randomizePositions(double const boundX,
                                 double const boundY);

         /// Updates the simulation world
         void update();

         /// Retrieve an animat
         model::Animat & animat(int const index);

       private:
         std::vector<model::Animat> m_animats;

         /// Move animat to new relative position in world
         void translateAnimatPosition(int const index,
                                      double const x, 
                                      double const y);
    };
}