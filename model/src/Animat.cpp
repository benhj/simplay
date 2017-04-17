/// Copyright (c) 2017 Ben Jones

#include "Animat.hpp"
#include "Spring.hpp"
#include "PointMass.hpp"
#include "Vector3.hpp"
#include "WaterForceGenerator.hpp"
#include <cmath>

namespace model {

    Animat::Animat(int const layers,
                   double const layerWidth,
                   double const blockHeight)
      : m_physicsEngine(layers * 2 /* number of point masses */)
      , m_antennaeMutex(std::make_shared<std::mutex>())
    {
        // construct layers
        auto yOffset = 0;
        auto xOffset = -(layerWidth / 2); // around zero
        for (int layer = 0; layer < layers; ++layer) {
            m_layers.emplace_back(xOffset, yOffset, layerWidth, m_physicsEngine);
            yOffset += blockHeight;
        }

        // construct blocks
        for (int block = 0; block < m_layers.size() - 1; ++block) {
            auto & layerA = m_layers[block];
            auto & layerB = m_layers[block + 1];
            m_blocks.emplace_back(layerA, 
                                  layerB,
                                  m_physicsEngine);
        }

        // construct bounding circles
        m_boundingCircles.reserve(layers - 1);
        for (int block = 0; block < m_layers.size() - 1; ++block) {
            m_boundingCircles.emplace_back(m_blocks[block].deriveBoundingCircle(m_physicsEngine));
        }

        // antennae
        constructAntennae();
    }

    void Animat::constructAntennae()
    {
        auto const PI = 3.14149;
        auto layerIndex = m_layers.size() - 1;
        auto & topLayer = m_layers[layerIndex];
        auto & secondLayer = m_layers[layerIndex - 1];
        auto leftIndex = topLayer.getIndexLeft();
        auto rightIndex = topLayer.getIndexRight();
        auto leftIndexSecond = secondLayer.getIndexLeft();
        auto rightIndexSecond = secondLayer.getIndexRight();
        auto leftPM = m_physicsEngine.getPointMassPosition(leftIndex);
        auto rightPM = m_physicsEngine.getPointMassPosition(rightIndex);
        auto leftPMSecond = m_physicsEngine.getPointMassPosition(leftIndexSecond);
        auto rightPMSecond = m_physicsEngine.getPointMassPosition(rightIndexSecond);

        auto dirLeft(leftPM - leftPMSecond);
        auto dirRight(rightPM - rightPMSecond);
        dirLeft.normalize();
        dirRight.normalize();
        physics::Vector3 upvec(0, 1, 0);
        upvec.normalize();

        auto baseAngle = acos((dirLeft.dot(upvec)));
        if (dirLeft.m_vec[0]<=0&&dirLeft.m_vec[1]<=0)baseAngle = -baseAngle-(PI/4);
        else if (dirLeft.m_vec[0]<=0&&dirLeft.m_vec[1]>=0)baseAngle = -baseAngle-(PI/4);
        else if (dirLeft.m_vec[0]>=0&&dirLeft.m_vec[1]<=0)baseAngle = baseAngle-(PI/4);
        else if (dirLeft.m_vec[0]>=0&&dirLeft.m_vec[1]>=0)baseAngle = baseAngle-(PI/4);
        auto ant = 4.0;
        auto cosBit = cos(baseAngle);
        auto sinBit = sin(baseAngle);
        auto xLeft = leftPM.m_vec[0] + (sinBit * ant);
        auto yLeft = leftPM.m_vec[1] + (cosBit * ant);

        double baseAngle2 = acos((dirRight.dot(upvec)));
        if (dirRight.m_vec[0]<=0&&dirRight.m_vec[1]<=0)baseAngle2 = -baseAngle2+(PI/4);
        else if (dirRight.m_vec[0]<=0&&dirRight.m_vec[1]>=0)baseAngle2 = -baseAngle2+(PI/4);
        else if (dirRight.m_vec[0]>=0&&dirRight.m_vec[1]<=0)baseAngle2 = baseAngle2+(PI/4);
        else if (dirRight.m_vec[0]>=0&&dirRight.m_vec[1]>=0)baseAngle2 = baseAngle2+(PI/4);
        auto cosBit2 = cos(baseAngle2);
        auto sinBit2 = sin(baseAngle2);

        auto xRight = rightPM.m_vec[0] + (sinBit2 * ant);
        auto yRight = rightPM.m_vec[1] + (cosBit2 * ant);

        std::lock_guard<std::mutex> lg(*m_antennaeMutex);
        m_leftAntenna.set(xLeft, yLeft, 0);
        m_rightAntenna.set(xRight, yRight, 0);
    }

    physics::Vector3 Animat::getLeftAntennaePoint() const
    {
        std::lock_guard<std::mutex> lg(*m_antennaeMutex);
        return m_leftAntenna;
    }
    physics::Vector3 Animat::getRightAntennaePoint() const
    {
        std::lock_guard<std::mutex> lg(*m_antennaeMutex);
        return m_rightAntenna;
    }

    void
    Animat::applyBlockContraction(int const block, 
                                  int const side, 
                                  double const force)
    {
        if (side == 0) {
            m_blocks[block].contractLeftSide(m_physicsEngine, force);
        } else {
            m_blocks[block].contractRightSide(m_physicsEngine, force);
        }
    }

    void
    Animat::relaxBlockContraction(int const block, int const side)
    {
        if (side == 0) {
            m_blocks[block].relaxLeftSide(m_physicsEngine);
        } else {
            m_blocks[block].relaxRightSide(m_physicsEngine);
        }
    }

    void Animat::applyWaterForces()
    {
        for (auto & block : m_blocks) {
            auto layerOne = block.getLayerOne();
            auto layerTwo = block.getLayerTwo();
            physics::WaterForceGenerator(layerOne, layerTwo, m_physicsEngine).apply();
        }
    }

    void Animat::updateBoundingCircles()
    {
        auto block = 0;
        for (auto & circle : m_boundingCircles) {
            auto bc = m_blocks[block].deriveBoundingCircle(m_physicsEngine);
            circle.first = bc.first;
            circle.second = bc.second;
            ++block;
        }
    }

    std::pair<physics::Vector3, double> 
    Animat::getBoundingCircle(int const index)
    {
        if (index > m_boundingCircles.size()) {
            throw std::runtime_error("Animat::getBoundingCircle: index out of bounds");
        }
        return m_boundingCircles[index];
    }

    void Animat::update()
    {
        m_physicsEngine.update(0.1);
        doUpdateDerivedComponents();
    }

    void Animat::updateDerivedComponents()
    {
        doUpdateDerivedComponents();
    }

    void  Animat::doUpdateDerivedComponents()
    {
        constructAntennae();
        updateBoundingCircles();
    }

    AnimatBlock const & Animat::getBlock(int const b) const
    {
        if (b > m_blocks.size()) {
            throw std::runtime_error("Animat::getBlock: index out of bounds");
        }
        return m_blocks[b];
    }

    AnimatLayer const & Animat::getLayer(int const layer) const
    {

        if (layer > m_layers.size()) {
            throw std::runtime_error("Animat::getLayer: index out of bounds");
        }
        return m_layers[layer];
    }

    physics::PhysicsEngine & Animat::getPhysicsEngine()
    {
        return m_physicsEngine;
    }

    int Animat::getBlockCount() const
    {
        return m_blocks.size();
    }

    physics::Vector3 Animat::getCentralPoint() const
    {
        // Rotate animat by heading matrix
        physics::Vector3 accumVector;

        // First update the point mass positions
        for (auto & layer : m_layers) {
            auto const indexLeft = layer.getIndexLeft();
            auto const indexRight = layer.getIndexRight();
            accumVector += m_physicsEngine.getPointMassPosition(indexLeft);
            accumVector += m_physicsEngine.getPointMassPosition(indexRight);
        }
        accumVector /= (m_layers.size() * 2);
        return accumVector;
    }
}
