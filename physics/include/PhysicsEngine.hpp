
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

        /// returns index of added mass point
        int addPointMass(PointMass const &pm);

        void setMassPointPosition(int const i, 
                                  Vector3 const & position);

        void createSpring(int const i, 
                          int const j,
                          double const springConstant,
                          double const dampener);
        void addSpring(Spring const & spring);

        Vector3 const & getMassPointPosition(int const i);

        void setPointForceExternal(int const i, 
                                   Vector3 const & force);

        Vector3 const & getPointForceAccel(int const i);
        Vector3 const & getPointForceExternal(int const i);
        void resetAllExternalForces();
        void updateSpringSystem();
        void resetForces();
        void reset();

        Vector3 const & getMassPointVelocity(int const i);

      private:
        /// Collection of point masses
        std::vector<PointMass> m_masses;

        /// Collection of springs
        std::vector<Spring> m_springs;
    };

}
