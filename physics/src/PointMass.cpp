/// Copyright (c) 2017 Ben Jones

#include "physics/PointMass.hpp"
#include <cmath>

namespace physics {

    PointMass::PointMass(Vector3 const & position, 
                         double const mass,
                         bool const frozen)
    : m_position(position)
    , m_initialPosition(position)
    , m_mass(mass)
    , m_frozen(frozen)
    , m_positionMutex(std::make_shared<std::mutex>())
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
        m_forceAccum[2] = 0;
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
        m_velocity.m_vec[2] = 0;
        {
            std::lock_guard<std::mutex> lg(*m_positionMutex);
            auto pos = m_position;
            pos += m_velocity * dt;
            pos.m_vec[2] = 0;
            auto isnan = false;
            if (!isnan)isnan = (std::isnan(pos.m_vec[0])||std::isinf(pos.m_vec[0]));
            if (!isnan)isnan = (std::isnan(pos.m_vec[1])||std::isinf(pos.m_vec[1]));
            if (!isnan)isnan = (std::isnan(pos.m_vec[2])||std::isinf(pos.m_vec[2]));
            if (!isnan) {
                m_position = pos;
            }
        }
        m_acceleration.toZero();
        m_forceAccum.toZero();
    }

    void PointMass::setMass(double const m)
    {
        m_mass = m;
    }

    void PointMass::setPosition(Vector3 const & pos)
    {
        std::lock_guard<std::mutex> lg(*m_positionMutex);
        bool isnan=false;
        if (!isnan)isnan = (std::isnan(pos.m_vec[0])||std::isinf(pos.m_vec[0]));
        if (!isnan)isnan = (std::isnan(pos.m_vec[1])||std::isinf(pos.m_vec[1]));
        if (!isnan)isnan = (std::isnan(pos.m_vec[2])||std::isinf(pos.m_vec[2]));
        if (!isnan) {
            m_position = pos;
        }
        m_position.m_vec[2] = 0;
    }

    Vector3 PointMass::lockedPosition(void) const
    {
        std::lock_guard<std::mutex> lg(*m_positionMutex);
        return m_position;
    }


    void PointMass::setVelocity(Vector3 const & vel)
    {
        bool isnan=false;
        if (!isnan)isnan = (std::isnan(vel.m_vec[0])||std::isinf(vel.m_vec[0]));
        if (!isnan)isnan = (std::isnan(vel.m_vec[1])||std::isinf(vel.m_vec[1]));
        if (!isnan)isnan = (std::isnan(vel.m_vec[2])||std::isinf(vel.m_vec[2]));
        if (!isnan) {
            m_velocity = vel;
        }
        m_velocity.m_vec[2] = 0;
    }


    void PointMass::setAcceleration(Vector3 const & acc)
    {
        bool isnan=false;
        if (!isnan)isnan = (std::isnan(acc.m_vec[0])||std::isinf(acc.m_vec[0]));
        if (!isnan)isnan = (std::isnan(acc.m_vec[1])||std::isinf(acc.m_vec[1]));
        if (!isnan)isnan = (std::isnan(acc.m_vec[2])||std::isinf(acc.m_vec[2]));
        if (!isnan) {
            m_acceleration = acc;
        }
        m_acceleration.m_vec[2] = 0;
    }

    void
    PointMass::reset()
    {
        m_velocity.toZero();
        m_acceleration.toZero();
        m_forceAccum.toZero();
    }

    void
    PointMass::toInitialPosition()
    {
        std::lock_guard<std::mutex> lg(*m_positionMutex);
        m_position = m_initialPosition;
        m_acceleration.toZero();
        m_forceAccum.toZero();
        m_velocity.toZero();
    }
}
