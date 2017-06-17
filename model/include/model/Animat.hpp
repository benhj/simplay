/// Copyright (c) 2017 Ben Jones
#pragma once

#include "AnimatLayer.hpp"
#include "AnimatBlock.hpp"
#include "AnimatProperties.hpp"

#include "physics/Vector3.hpp"
#include "physics/PhysicsEngine.hpp"

#include <vector>
#include <mutex>
#include <memory>

namespace model {

    class Animat
    {

      public:
        /// Initialize animat with default properties (10 segments
        /// a with of 2.0 and a height of 3.8611)
        Animat(AnimatProperties const & props = {10, 2.0, 3.8611});
   
        void applyBlockContraction(int const block, 
                                   int const side, 
                                   double const force);

        void relaxBlockContraction(int const block, 
                                   int const side);

        void applyWaterForces();

        /// Integrates animat physics
        void update();

        /// To update the derived components (antenna, bounding circles etc.).
        /// Usually a call to this won't ne necessary as derivation will happen
        /// as part of the update() function. ight be called during animat
        /// initialization when position is randomly set.
        void updateDerivedComponents();

        AnimatBlock & getBlock(int const b);
        AnimatLayer & getLayer(int const layer);

        physics::PhysicsEngine & getPhysicsEngine();

        physics::Vector3 getLeftAntennaePoint() const;
        physics::Vector3 getRightAntennaePoint() const;

        /// Retrieve number of blocks or body segments
        int getBlockCount() const;

        /// Updated after each physics update
        void updateBoundingCircles();

        /// Retrieve a given bounding circle
        std::pair<physics::Vector3, double> getBoundingCircle(int const index);

        /// Computes the central point of the animat
        /// (point, radius)
        std::pair<physics::Vector3, double>  getCentralPoint() const;

        /// Indicates if physics broke
        bool broke() const;

      private:
        std::vector<AnimatLayer> m_layers;
        std::vector<AnimatBlock> m_blocks;
        physics::PhysicsEngine m_physicsEngine;
        physics::Vector3 m_leftAntenna;
        physics::Vector3 m_rightAntenna;
        std::pair<physics::Vector3, double> m_centralPoint;
        mutable std::shared_ptr<std::mutex> m_antennaeMutex;
        mutable std::shared_ptr<std::mutex> m_centralPointMutex;

        /// To indicate if the physics became unstable during an update
        mutable bool m_physicsBecameUnstable;

        /// Useful for collision detection and resolution
        std::vector<std::pair<physics::Vector3, double>> m_boundingCircles;
        void constructAntennae();

        /// Derived components are those whose geometry are calculated
        /// from pre-existing point mass information
        void doUpdateDerivedComponents();

        /// Useful for the 'large' bounding circle arround the whole
        /// animat to be used for course collision detection
        /// and resolution
        void updateCentralPoint();

        /// When the animat's physics have become so unstable
        /// that it needs resetting
        bool totallyBuggered() const;

        /// To reset the animat structure after bad physics
        void resetAnimatStructure();

    };
}

