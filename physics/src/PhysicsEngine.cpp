
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

    void PhysicsEngine::compressSpring(int const index,
                                       double const forceMagnitude)
    {
        if (index >= m_springs.size()) {
            throw std::runtime_error("compressSpring: index out of bounds");
        }
        m_springs[index].compress(forceMagnitude);
    }

    void
    PhysicsEngine::setPointMassPosition(int const i, Vector3 const & position)
    {
        m_masses[i].setPosition(position);
    }

    Vector3 const & PhysicsEngine::getPointMassPosition(int const i) const
    {
        return m_masses[i].position();
    }

    void PhysicsEngine::setPointForceExternal(int const p,
                                              Vector3 const & u)
    {
        m_masses[p].accumulateForce(u);
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

    std::vector<PointMass> PhysicsEngine::getMasses() const
    {
        return m_masses;
    }

}
