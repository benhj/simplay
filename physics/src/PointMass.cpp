#include "PointMass.hpp"
#include <cmath>

namespace physics {

    PointMass::PointMass(Vector3 const & position, 
                         double const mass,
                         bool const frozen)
    : m_position(position)
    , m_mass(mass)
    , m_frozen(frozen)
    {

    }

    Vector3 const &
    PointMass::getForceAccum()
    {
        return m_forceAccum;
    }

    void PointMass::accumulateForce(Vector3 const & force)
    {
        m_forceAccum += force;
    }

    void PointMass::applyForce()
    {
        if (m_mass != 0) {
            m_acceleration += (m_forceAccum / m_mass);
            m_acceleration.m_vec[2] = 0;
        }
    }


    // Integrator
    void PointMass::update(double const dt)
    {
        applyForce();
        m_velocity += m_acceleration * dt;
        m_position += m_velocity * dt;
        m_acceleration.toZero();
        m_forceAccum.toZero();
    }

    void PointMass::setMass(double const m)
    {
        m_mass = m;
    }

    void PointMass::setPosition(Vector3 const & pos)
    {
        m_position = pos;
    }


    void PointMass::setVelocity(Vector3 const & vel)
    {
        m_velocity = vel;
    }


    void PointMass::setAcceleration(Vector3 const & acc)
    {
        m_acceleration = acc;
    }

    void
    PointMass::reset()
    {
        m_velocity.toZero();
        m_acceleration.toZero();
        m_forceAccum.toZero();
    }
}
