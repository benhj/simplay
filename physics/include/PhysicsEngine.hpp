/// Copyright (c) 2017 Ben Jones
#pragma once

#include "Spring.hpp"
#include <vector>

namespace physics {

    class PointMass;
    class Vector3;

    class PhysicsEngine
    {
      public:
        explicit PhysicsEngine(int const pointCount);
        PhysicsEngine() = delete;

        /// returns index of added mass point
        int addPointMass(Vector3 const & position, 
                         double const mass, 
                         bool const fixed);

        void setPointMassPosition(int const i, 
                                  Vector3 const & position);

        /// returns index of spring
        int createSpring(int const i, 
                         int const j,
                         double const springConstant,
                         double const dampener);

        void compressSpring(int const index, double const forceMagnitude);
        void relaxSpring(int const index);

        Vector3 getPointMassPosition(int const i) const;
        Vector3 getPointMassVelocity(int const i) const;

        void setPointForceExternal(int const i,  Vector3 const & force);

        void resetAllExternalForces();

        /// integrate
        void update(double const dv);
        void reset();

      private:
        /// Collection of point masses
        mutable std::vector<PointMass> m_masses;

        /// Collection of springs
        std::vector<Spring> m_springs;
    };

}
