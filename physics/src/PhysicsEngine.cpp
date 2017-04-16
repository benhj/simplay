/// Copyright (c) 2017 Ben Jones

#include "PhysicsEngine.hpp"
#include "PointMass.hpp"
#include "Vector3.hpp"

namespace physics {

    PhysicsEngine::PhysicsEngine(int const pointCount)
    : m_masses()
    , m_springs()
    {
        auto const blockCount = (pointCount / 2) - 1;
        auto const springCount = ((blockCount * 4) + blockCount + 1);
        m_masses.reserve(pointCount);
        m_springs.reserve(springCount);
    }

    int PhysicsEngine::addPointMass(Vector3 const & position,
                                    double const mass, 
                                    bool const fixed)
    {
        m_masses.emplace_back(position, mass, fixed);
        return m_masses.size() - 1;
    }

    void PhysicsEngine::setPointForceExternal(int const i,  Vector3 const & force)
    {
        if (i >= m_masses.size()) {
            throw std::runtime_error("setPointForceExternal: i out of bounds");
        }
        m_masses[i].accumulateForce(force);
    }

    int
    PhysicsEngine::createSpring(int const i, 
                                int const j,
                                double const springConstant,
                                double const dampener)
    {
        if (i >= m_masses.size()) {
            throw std::runtime_error("createSpring: i out of bounds");
        }
        if (j >= m_masses.size()) {
            throw std::runtime_error("createSpring: j out of bounds");
        }
        m_springs.emplace_back(std::ref(m_masses[i]), std::ref(m_masses[j]), springConstant, dampener);
        return m_springs.size() - 1;
    }

    void PhysicsEngine::compressSpring(int const index,
                                       double const forceMagnitude)
    {
        if (index >= m_springs.size()) {
            throw std::runtime_error("compressSpring: index out of bounds");
        }
        m_springs[index].compress(forceMagnitude);
    }

    void PhysicsEngine::relaxSpring(int const index)
    {
        if (index >= m_springs.size()) {
            throw std::runtime_error("relaxSpring: index out of bounds");
        }
        m_springs[index].relax();
    }

    void
    PhysicsEngine::setPointMassPosition(int const i, Vector3 const & position)
    {
        m_masses[i].setPosition(position);
    }

    Vector3 PhysicsEngine::getPointMassPosition(int const i) const
    {
        return m_masses[i].lockedPosition();
    }

    Vector3 PhysicsEngine::getPointMassVelocity(int const i) const
    {
        return m_masses[i].velocity();
    }

    Vector3 & PhysicsEngine::getPointMassPositionRef(int const i)
    {
        return m_masses[i].position();
    }

    void PhysicsEngine::update(double const dv)
    {
        std::for_each(std::begin(m_springs), 
                      std::end(m_springs), 
                      [](Spring & s) { s.apply(); });

        std::for_each(std::begin(m_masses),
                      std::end(m_masses),
                      [=](PointMass & pm) { pm.update(dv); });
    }

    void PhysicsEngine::reset()
    {
        std::for_each(std::begin(m_masses), 
                      std::end(m_masses), 
                      [](PointMass & p) { p.reset(); });
    }
}
