/// Copyright (c) 2017 Ben Jones

#include "Spring.hpp"
#include <cmath>
#include <cassert>

namespace physics {

    Spring::Spring(PointMass & p0, 
                   PointMass & p1, 
                   double const k,
                   double const dampener)
      : m_p0(p0)
      , m_p1(p1)
      , m_springConstant(k)
      , m_dampener(dampener)
      , m_restLength(m_p0.lockedPosition().distance(m_p1.lockedPosition()))
      , m_fixedP0(false)
      , m_fixedP1(false)
    {
    }

    
    void Spring::setFixedPoints(bool const p0_fixed, 
                                bool const p1_fixed)
    {
        m_fixedP0 = p0_fixed;
        m_fixedP1 = p1_fixed;
    }

    void Spring::setSpringConstant(double const k)
    {
        m_springConstant = k;
    }

    void Spring::setDampener(double const d)
    {
        m_dampener = d;
    }

    void Spring::apply(void)
    {
        //if (!rigid)
        {
            auto v = m_p1.lockedPosition() - m_p0.lockedPosition();
            auto vlength = v.length();
            auto forceMagnitude = m_springConstant * (vlength - m_restLength);
            if (vlength > 0) {
                v /= vlength;
            }
            v *= forceMagnitude;

            // // apply dampening to each point which is -d * velocity
            auto & p0_vel = m_p0.velocity();
            auto & p1_vel = m_p1.velocity();
            auto damped0 = -m_dampener * p0_vel;
            auto damped1 = -m_dampener * p1_vel;

            if (!m_fixedP0) {
                m_p0.accumulateForce(v + damped0);
            }
            if (!m_fixedP1) {
                m_p1.accumulateForce(-v + damped1);
            }
        } 
        /*
        else { // move point 1 exaclty in relation to point 0. I.e. distance is maintained.
            p1Ptr.setVelocity(p0Ptr.velocity());
            p1Ptr.setAcceleration(p0Ptr.acceleration());
        }*/
    }

    void Spring::compress(double const forceMagnitude)
    {
        m_compressForceP0 = m_p1.lockedPosition() - m_p0.lockedPosition(); 
        m_compressForceP1 = m_p0.lockedPosition() - m_p1.lockedPosition();
        m_compressForceP0 *= forceMagnitude;
        m_compressForceP1 *= forceMagnitude;
        m_p0.accumulateForce(m_compressForceP0);
        m_p1.accumulateForce(m_compressForceP1);
    }

    void Spring::relax()
    {
        m_p0.accumulateForce(-m_compressForceP0);
        m_p1.accumulateForce(-m_compressForceP1);
    }

    double
    Spring::getCurrentDistension()
    {
        auto v = m_p1.lockedPosition() - m_p0.lockedPosition();
        return m_restLength - v.length();
    }
}
