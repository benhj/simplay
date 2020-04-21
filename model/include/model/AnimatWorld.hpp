/// Copyright (c) 2017-present Ben Jones

#pragma once

/// For describing a population of animats in a given world.
/// Responsible for initializing and updating the world.

#include "Animat.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace model {
    class AnimatWorld
    {
       public:
         AnimatWorld(int const populationSize);
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
         std::shared_ptr<model::Animat> animat(int const index);

         int getPopSize() const;

         /// If the animat falls out of the designated
         /// bounds of the environment, 'wrap' it around
         /// in the manner of a toroid.
         void translateIfOutOfBounds(int const index,
                                     double const boundX,
                                     double const boundY);

         /// Rebuild a given animat with an updated number
         /// of body segments.
         void reconstructAnimat(int const index, int const segments);

         /// The observer that gets triggered before and after updating
         /// the physical morphology of a given animat so that the
         /// graphics subsystem can be appropriately locked
         void setAnimatUpdatedObserver
         (std::function<void(int const, std::shared_ptr<Animat>)> func);

         static std::vector<std::shared_ptr<std::mutex>> g_fuckers;

         void incrementOptimizationCount();
         long getOptimizationCount() const;

       private:
         std::vector<std::shared_ptr<model::Animat>> m_animats;

         /// Before updating an animat's geometry, this function
         /// should be called to lock the graphics observer
         /// and called againt afterwards to unlock it
         std::function<void(int const, std::shared_ptr<Animat>)> 
         m_animatUpdatedObserver;

         /// Records the number of times the population
         /// has been optimized. Equivalent to the concept
         /// of 'generation'.
         long m_optimizations;

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