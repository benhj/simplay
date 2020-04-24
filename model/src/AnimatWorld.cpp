/// Copyright (c) 2017-present Ben Jones

#include "model/AnimatWorld.hpp"
#include "physics/Matrix.hpp"
#include <cstdlib> // rand
#include <ctime> // time
#include <cmath>
#include <utility>

namespace model {

    std::vector<std::shared_ptr<std::mutex>> AnimatWorld::g_fuckers;

    AnimatWorld::AnimatWorld(int const populationSize)
      : m_animats()
      , m_animatUpdatedObserver()
      , m_optimizations(0)
    {
        m_animats.reserve(populationSize);

        for (int p = 0; p < populationSize; ++p) {
            m_animats.push_back(std::make_shared<Animat>(p));
        }

        // seed random generator for random pop placement
        ::srand(::time(NULL));

        if(g_fuckers.empty()) {
            g_fuckers.push_back(std::make_shared<std::mutex>());
        }

    }


    void AnimatWorld::incrementOptimizationCount()
    {
        ++m_optimizations;
    }
    long AnimatWorld::getOptimizationCount() const
    {
        return m_optimizations;
    }

    int AnimatWorld::getPopSize() const
    {
        return m_animats.size();
    }

    void AnimatWorld::reconstructAnimat(int const index, int const segments)
    {
        /*
        if(m_animatUpdatedObserver) {
            m_animatUpdatedObserver(index);
        }*/
        //std::lock_guard<std::mutex> lg(*g_fuckers[0]);
        auto newAnimat = std::make_shared<Animat>(index, AnimatProperties{segments, 2.0, 4.0});
        m_animats[index] = newAnimat;
        if(m_animatUpdatedObserver) {
            m_animatUpdatedObserver(index, m_animats[index]);
        }
    }

    void AnimatWorld::setAnimatUpdatedObserver
    (std::function<void(int const, std::shared_ptr<Animat>)> func)
    {
        m_animatUpdatedObserver = std::move(func);
    }

    bool AnimatWorld::nearAnotherAnimat(int const index)
    {
        auto centralPointIndex = m_animats[index]->getCentralPoint();
        for (int i = 0; i < index; ++i) {
            auto centralPointOther = m_animats[i]->getCentralPoint();
            if (centralPointIndex.first.distance(centralPointOther.first) 
                < centralPointIndex.second + centralPointOther.second) {
                return true;
            }
        }
        for (int i = m_animats.size() - 1; i > index; --i) {
            auto centralPointOther = m_animats[i]->getCentralPoint();
            if (centralPointIndex.first.distance(centralPointOther.first) 
                < centralPointIndex.second + centralPointOther.second) {
                return true;
            }
        }
        return false;
    }

    void AnimatWorld::randomizePositions(double const boundX,
                                         double const boundY)
    {
        for (auto i = 0; i < m_animats.size(); ++i) {
            m_animats[i]->resetAnimatStructure();
            doRandomizePosition(i, boundX, boundY);
            while (nearAnotherAnimat(i)) {
                doRandomizePosition(i, boundX, boundY);
            }
        }
    }

    void AnimatWorld::randomizePositionSingleAnimat(int const index,
                                                    double const boundX,
                                                    double const boundY)
    {
        doRandomizePosition(index, boundX, boundY);
        while (nearAnotherAnimat(index)) {
            doRandomizePosition(index, boundX, boundY);
        }
    }

    void AnimatWorld::doRandomizePosition(int const index,
                                          double const boundX,
                                          double const boundY)
    {
        auto randomX = ((double) rand() / (RAND_MAX)) * boundX;
        auto randomY = ((double) rand() / (RAND_MAX)) * boundY;
        auto posOrNeg = ((double) rand() / (RAND_MAX));
        if (posOrNeg >= 0.5) {
            randomX = -randomX;
        }
        posOrNeg = ((double) rand() / (RAND_MAX));
        if (posOrNeg >= 0.5) {
            randomY = -randomY;
        }
        auto cp = m_animats[index]->getCentralPoint().first[0];
        doTranslateAnimatPosition(index, randomX, randomY);
        cp = m_animats[index]->getCentralPoint().first[0];
        auto angle = ((double) rand() / (RAND_MAX)) * (3.14159265 * 2);
        doSetHeading(index, angle);
    }

    void AnimatWorld::doSetHeading(int const index,
                                   double const angle)
    {
        // Derive heading matrix given angle
        physics::Matrix xAxis, yAxis, zAxis;
        xAxis.toIdentity();
        yAxis.toIdentity();
        zAxis.toIdentity();
        xAxis[1][1]=cos(0);
        xAxis[1][2]=-sin(0);
        xAxis[2][1]=sin(0);
        xAxis[2][2]=cos(0);
        yAxis[0][0]=cos(0);
        yAxis[0][2]=sin(0);
        yAxis[2][0]=-sin(0);
        yAxis[2][2]=cos(0);
        zAxis[0][0]=cos(angle);
        zAxis[0][1]=-sin(angle);
        zAxis[1][0]=sin(angle);
        zAxis[1][1]=cos(angle);
        auto zy = zAxis * yAxis;
        auto headingMatrix = zy * xAxis;

        // Rotate around zero so need to offset
        auto & animat = m_animats[index];
        auto & physicsEngine = animat->getPhysicsEngine();
        auto centerPoint = animat->getCentralPoint();
        auto difference = physicsEngine.getPointMassPosition(animat->getLayer(0).getIndexLeft())
                        - centerPoint.first;
        doTranslateAnimatPosition(index, difference.m_vec[0], difference.m_vec[1]);

        // Rotate animat by heading matrix
        auto const blockCount = animat->getBlockCount();

        // First update the point mass positions
        for (auto lay = 0 ; lay <= blockCount; ++lay) {
            auto & layer = animat->getLayer(lay);
            auto const indexLeft = layer.getIndexLeft();
            auto const indexRight = layer.getIndexRight();
            auto & leftPos = physicsEngine.getPointMassPositionRef(indexLeft);
            auto & rightPos = physicsEngine.getPointMassPositionRef(indexRight);
            leftPos *= headingMatrix;
            rightPos *= headingMatrix;
        }

        // ...and translate back again
        doTranslateAnimatPosition(index, -difference.m_vec[0], -difference.m_vec[1]);

    }

    void AnimatWorld::update()
    {
        for (auto & animat : m_animats) {
            animat->update();
        }
    }

    std::shared_ptr<model::Animat>AnimatWorld::animat(int const index)
    {
        return m_animats[index];
    }

    void AnimatWorld::doTranslateAnimatPosition(int const index,
                                                double const x, 
                                                double const y)
    {
        auto & animat = m_animats[index];
        auto & physicsEngine = animat->getPhysicsEngine();
        auto const blockCount = animat->getBlockCount();

        // First update the point mass positions
        for (auto lay = 0 ; lay <= blockCount; ++lay) {
            auto & layer = animat->getLayer(lay);
            auto const indexLeft = layer.getIndexLeft();
            auto const indexRight = layer.getIndexRight();
            auto & leftPos = physicsEngine.getPointMassPositionRef(indexLeft);
            auto & rightPos = physicsEngine.getPointMassPositionRef(indexRight);
            leftPos.m_vec[0] += x;
            leftPos.m_vec[1] += y;
            rightPos.m_vec[0] += x;
            rightPos.m_vec[1] += y;
        }

        // Then update the derived stuff (antenna, bounding circles etc.)
        animat->updateDerivedComponents();
    }

    void AnimatWorld::translateIfOutOfBounds(int const index,
                                              double const boundX,
                                              double const boundY)
    {
        auto centralPoint = m_animats[index]->getCentralPoint();
        auto transX = 0.0;
        auto transY = 0.0;
        if(centralPoint.first[0] < -boundX) {
            transX = (boundX * 2);
        } else if(centralPoint.first[0] > boundX) {
            transX = -(boundX * 2);
        }
        if(centralPoint.first[1] < -boundY) {
            transY = (boundY * 2);
        } else if(centralPoint.first[1] > boundY) {
            transY = -(boundY * 2);
        }
        if(transX != 0 || transY != 0) {
            m_animats[index]->setWrapped();
            doTranslateAnimatPosition(index, transX, transY);
        }
    }
}