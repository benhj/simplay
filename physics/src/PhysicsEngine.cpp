
#include "PhysicsEngine.hpp"
#include "Spring.hpp"
#include "PointMass.hpp"
#include "Vector3.hpp"

namespace physics {

    PhysicsEngine::PhysicsEngine()
    {
    }

    int PhysicsEngine::addPointMass(Vector3 const & position,
                                double const mass, 
                                bool const fixed)
    {
        m_masses.emplace_back(position, mass, fixed);
        return m_masses.size() - 1;
    }

    int PhysicsEngine::addPointMass(PointMass const &pm)
    {
        m_masses.push_back(pm);
        return m_masses.size() - 1;
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
        m_springs.emplace_back(m_masses[i], m_masses[j], springConstant, dampener);
        return m_springs.size() - 1;
    }

    void PhysicsEngine::addSpring(Spring const & spring) 
    {
        m_springs.push_back(spring);
    }

    void PhysicsEngine::compressSpring(int const index,
                                       double const forceMagnitude)
    {
        if (index >= m_springs.size()) {
            throw std::runtime_error("compressSpring: index out of bounds");
        }
        m_springs[index].compress(forceMagnitude);
    }

    void
    PhysicsEngine::setMassPointPosition(int const i, Vector3 const & position)
    {
        m_masses[i].setPosition(position);
    }

    Vector3 const & PhysicsEngine::getMassPointPosition(int const i) const
    {
        return m_masses[i].position();
    }

    Vector3 const & PhysicsEngine::getMassPointVelocity(int const i) const
    {
        return m_masses[i].velocity();
    }

    void PhysicsEngine::setPointForceExternal(int const p,
                                              Vector3 const & u)
    {
        m_masses[p].accumulateForce(u);
    }

    Vector3 const & PhysicsEngine::getPointForceExternal(int const p) const
    {
        return m_masses[p].getForceAccum();
    }

    Vector3 const & PhysicsEngine::getPointForceAccel(int const p) const
    {
        return m_masses[p].acceleration();
    }

    void PhysicsEngine::update(double const dv)
    {
        std::for_each(std::begin(m_masses),
                      std::end(m_masses),
                      [=](PointMass & pm) { pm.update(dv); });

        std::for_each(std::begin(m_springs), 
                      std::end(m_springs), 
                      [](Spring & s) { s.apply(); });
    }

    void PhysicsEngine::reset()
    {
        std::for_each(std::begin(m_masses), 
                      std::end(m_masses), 
                      [](PointMass & p) { p.reset(); });
    }

}
