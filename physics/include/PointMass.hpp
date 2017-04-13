/// Copyright (c) 2017 Ben Jones
#pragma once

/**
 * Based on code found at http://resumbrae.com/ub/dms424_s03/
 */

#include "Vector3.hpp"

#include <vector>
#include <mutex>
#include <memory>

namespace physics {

    class PointMass
    {
      public:
        int id;
        PointMass(Vector3 const & pos = Vector3::Zero, 
                  double const mass = 1.0,
                  bool const frozen = false);

        void accumulateForce(const Vector3 & force);
        void applyForce();
        void update(double const dt);

        inline double mass(void) { return m_mass; }
        inline const Vector3& position(void) { return m_position; }
        Vector3 lockedPosition(void) const;
        inline const Vector3& velocity(void) { return m_velocity; }
        inline const Vector3& acceleration(void) { return m_acceleration; }

        void setMass(double const m);
        void setPosition(Vector3 const & pos);
        void setVelocity(Vector3 const & vel);
        void setAcceleration(Vector3 const & acc);

        Vector3 const & getForceAccum();

        void reset();

      private:

        Vector3 m_position;
        double m_mass;
        bool m_frozen;
        
        Vector3 m_velocity;
        Vector3 m_acceleration;
        Vector3 m_forceAccum;

        // Although all point masses will be unique,
        // we need to store them in a vector meaning that
        // they need to be copyable.
        mutable std::shared_ptr<std::mutex> m_positionMutex;
        
    };

}

