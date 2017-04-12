
#pragma once

#include <vector>

namespace physics {

    class Spring;
    class PointMass;
    class Vector3;

    class PhysicsEngine
    {
      public:
        PhysicsEngine();

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

        Vector3 const & getPointMassPosition(int const i) const;

        void setPointForceExternal(int const i,  Vector3 const & force);

        void resetAllExternalForces();

        /// integrate
        void update(double const dv);
        void reset();

        /// hack. Needs to be made thread-safe
        std::vector<PointMass> getMasses() const;

      private:
        /// Collection of point masses
        mutable std::vector<PointMass> m_masses;

        /// Collection of springs
        std::vector<Spring> m_springs;

        PointMass *debugMass;
    };

}
