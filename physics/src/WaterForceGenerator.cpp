/// Copyright (c) 2017 Ben Jones

#include "WaterForceGenerator.hpp"

namespace physics {

    WaterForceGenerator::WaterForceGenerator(model::AnimatLayer & layerOne,
                                             model::AnimatLayer & layerTwo,
                                             PhysicsEngine & physicsEngine)
      : m_layerOne(layerOne)
      , m_layerTwo(layerTwo)
      , m_physicsEngine(physicsEngine)
    {
    }

    Vector3
    WaterForceGenerator::computeFaceNormalLeft() const
    {
        auto layerOneLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto layerTwoLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto distance = layerOneLeftPosition - layerTwoLeftPosition;
        auto pv = distance / 2;
        auto rv = pv + layerTwoLeftPosition;
        auto xp = rv.m_vec[0];
        auto yp = rv.m_vec[1];
        auto const dx = layerTwoLeftPosition.m_vec[0] - layerOneLeftPosition.m_vec[0];
        auto const dy = layerTwoLeftPosition.m_vec[1] - layerOneLeftPosition.m_vec[1];
        auto normalX = dy + xp;
        auto normalY = -dx + yp;
        Vector3 thisone(normalX, normalY, 0);
        Vector3 otherone(xp, yp, 0);
        Vector3 normal(thisone - otherone);
        normal.normalize();
        return normal;
    }

    Vector3
    WaterForceGenerator::computeFaceNormalRight() const
    {
        auto layerOneRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto layerTwoRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto distance = layerOneRightPosition - layerTwoRightPosition;
        auto pv = distance / 2;
        auto rv = pv + layerTwoRightPosition;
        auto xp = rv.m_vec[0];
        auto yp = rv.m_vec[1];
        auto const dx = layerTwoRightPosition.m_vec[0] - layerOneRightPosition.m_vec[0];
        auto const dy = layerTwoRightPosition.m_vec[1] - layerOneRightPosition.m_vec[1];
        auto normalX = -dy + xp;
        auto normalY = dx + yp;
        Vector3 thisone(normalX, normalY, 0);
        Vector3 otherone(xp, yp, 0);
        Vector3 normal(thisone - otherone);
        normal.normalize();
        return normal;
    }

    double
    WaterForceGenerator::computeLeftSideArea() const
    {
        auto layerOneLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto layerTwoLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto difference = layerOneLeftPosition - layerTwoLeftPosition;
        auto length = difference.length();
        return length * 10.0;
    }

    double
    WaterForceGenerator::computeRightSideArea() const
    {
        auto layerOneRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto layerTwoRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto difference = layerOneRightPosition - layerTwoRightPosition;
        auto length = difference.length();
        return length * 10.0;
    }

    void WaterForceGenerator::apply()
    {
        // Coefficients
        auto nFactor(6.0);
        auto tFactor(0.0875);

        // get normal components
        auto normalLeft = computeFaceNormalLeft();
        auto normalRight = computeFaceNormalRight();
        auto tangentLeft = getTangentLeft();
        auto tangentRight = getTangentLeft();

        // get average velocities
        auto velocityLeftLayerOne = m_layerOne.getVelocityLeft(m_physicsEngine);
        auto velocityLeftLayerTwo = m_layerTwo.getVelocityLeft(m_physicsEngine);
        auto velocityRightLayerOne = m_layerOne.getVelocityRight(m_physicsEngine);
        auto velocityRightLayerTwo = m_layerTwo.getVelocityRight(m_physicsEngine);
        auto leftFaceVelocity = (velocityLeftLayerOne + velocityLeftLayerTwo) / 2.0;
        auto rightFaceVelocity = (velocityRightLayerOne + velocityRightLayerTwo) / 2.0;

        // compute left and right normal parts
        auto normalComponentLeft = normalLeft.dot(leftFaceVelocity);
        auto normalComponentRight = normalRight.dot(rightFaceVelocity);
        auto normalComponentSgnLeft = sgn(normalComponentLeft);
        auto normalComponentSgnRight = sgn(normalComponentRight);
        auto normalComponentSQLeft = normalComponentLeft * normalComponentLeft;
        auto normalComponentSQRight = normalComponentRight * normalComponentRight;
        auto normalPartLeft = -nFactor * normalComponentSgnLeft * normalComponentSQLeft;
        auto normalPartRight = -nFactor * normalComponentSgnRight * normalComponentSQRight;

        // compute tangential parts
        auto tangentComponentLeft = tangentLeft.dot(leftFaceVelocity);
        auto tangentComponentRight = tangentRight.dot(rightFaceVelocity);
        auto tangentComponentSgnLeft = sgn(tangentComponentLeft);
        auto tangentComponentSgnRight = sgn(tangentComponentRight);
        auto tangentComponentSQLeft = tangentComponentLeft * tangentComponentLeft;
        auto tangentComponentSQRight = tangentComponentRight * tangentComponentRight;
        auto tangentPartLeft = -tFactor * tangentComponentSgnLeft * tangentComponentSQLeft;
        auto tangentPartRight = -tFactor * tangentComponentSgnRight * tangentComponentSQRight;

        // compute overall forces to apply
        auto compedForceLeft = (normalPartLeft * normalLeft) + (tangentPartLeft * tangentLeft);
        auto compedForceRight = (normalPartRight * normalRight) + (tangentPartRight * tangentRight);
        auto areaLeft = computeLeftSideArea();
        auto areaRight = computeRightSideArea();
        compedForceLeft *= areaLeft;
        compedForceRight *= areaRight;

        // apply forces
        auto indexLeftLayerOne = m_layerOne.getIndexLeft();
        auto indexLeftLayerTwo = m_layerTwo.getIndexLeft();
        auto indexRightLayerOne = m_layerOne.getIndexRight();
        auto indexRightLayerTwo = m_layerTwo.getIndexRight();
        m_physicsEngine.setPointForceExternal(indexLeftLayerOne, compedForceLeft);
        m_physicsEngine.setPointForceExternal(indexLeftLayerTwo, compedForceLeft);
        m_physicsEngine.setPointForceExternal(indexRightLayerOne, compedForceRight);
        m_physicsEngine.setPointForceExternal(indexRightLayerTwo, compedForceRight);
    }

    Vector3
    WaterForceGenerator::getTangentLeft() const
    {
        auto layerOneLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto layerTwoLeftPosition = m_layerOne.getPositionLeft(m_physicsEngine);
        auto firstApprox = layerOneLeftPosition - layerTwoLeftPosition;
        firstApprox.normalize();
        return firstApprox;
    }

    Vector3
    WaterForceGenerator::getTangentRight() const
    {
        auto layerOneRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto layerTwoRightPosition = m_layerOne.getPositionRight(m_physicsEngine);
        auto firstApprox = layerOneRightPosition - layerTwoRightPosition;
        firstApprox.normalize();
        return firstApprox;
    }

    double
    WaterForceGenerator::sgn(double const in) const
    {
        if (in<0)return -1;
        else if (in>0)return 1;
        else return 0;
    }
}