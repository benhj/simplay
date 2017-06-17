/// Copyright (c) 2017 Ben Jones

#pragma once

/// For describing a population of animats in a given world.
/// Responsible for initializing and updating the world.

#include "model/Animat.hpp"
#include "model/AnimatProperties.hpp"
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

         void randomizePositionSingleAnimat(int const index,
                                            double const boundX,
                                            double const boundY);

         /// Updates the simulation world
         void update();

         /// Retrieve an animat
         model::Animat & animat(int const index);

         int getPopSize() const;

       private:
         std::vector<model::Animat> m_animats;

         /// Move animat to new relative position in world
         void doTranslateAnimatPosition(int const index,
                                        double const x, 
                                        double const y);

         /// Randomize the position of a single animat
         void doRandomizePosition(int const index,
                                  double const x,
                                  double const y);

         /// Rotates animat along the z-axis
         void doSetHeading(int const index, double const angle);

         /// Check if animat at index overlaps another
         bool nearAnotherAnimat(int const index);
    };
}