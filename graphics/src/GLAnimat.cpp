// Copyright (c) 2017-present Ben Jones

#include "glfreetype/TextRenderer.hpp"
#include "graphics/GLAnimat.hpp"
#include "graphics/WorldToScreen.hpp"
#include "graphics/RetinaScalar.hpp"
#include "physics/PhysicsEngine.hpp"
#include "model/Animat.hpp"
#include "model/AnimatWorld.hpp"
#include <OpenGL/gl.h>
#include <cmath>
#include <atomic>
#include <memory>
#include <mutex>
#include <sstream>
#include <unistd.h>

namespace {
    glfreetype::font_data g_animatText;
    std::once_flag g_textInitGuard;
}

namespace graphics {

    namespace detail {

        inline void drawCircle(float cx, float cy, float r, int num_segments, bool const filled = false) 
        {
            if (!filled) {
                glBegin(GL_LINE_LOOP);
            } else {
                glBegin(GL_POLYGON);
            }
            for (int ii = 0; ii < num_segments; ii++)   {
                float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
                float x = r * ::cosf(theta);//calculate the x component 
                float y = r * ::sinf(theta);//calculate the y component 
                glVertex3f(x + cx, y + cy, 0);//output vertex 
            }
            glEnd();
        }
    }

    GLAnimat::GLAnimat(std::shared_ptr<model::Animat> animat,
                       detail::ThreadRunner & threadRunner)
      : m_animat(std::move(animat))
      , m_threadRunner(threadRunner)
      , m_highlighted(std::make_shared<std::atomic<bool>>(false)) 
      , m_selected(std::make_shared<std::atomic<bool>>(false))
      , m_opacity(std::make_shared<std::atomic<double>>(1))
    {
        std::call_once(g_textInitGuard, 
                       [&](){
                           g_animatText.init("../fonts/Action_Man.ttf",
                           25 * detail::retinaScalar()); 
                        });
    }

    void GLAnimat::updateAnimat(std::shared_ptr<model::Animat> animat)
    {
        m_animat = std::move(animat);
    }

    void GLAnimat::draw()
    {
        detail::setColor(m_basicColor);
        drawBody();
        //drawAntennae();
        drawBoundingCircles();
        if (*m_highlighted || *m_selected) {
            drawBigBoundingCircle();
        }
    }

    bool GLAnimat::handleSelection()
    {
        if(*m_highlighted) {
            *m_selected = true;
        } else {
            *m_selected = false;
        }
        return *m_selected;
    }

    void GLAnimat::checkForHighlight(double x, 
                                     double y,
                                     double const viewDistance,
                                     double const centerX,
                                     double const centerY)
    {
        // offset mouse coords
        y -= centerY;
        x -= centerX;
        x *= detail::retinaScalar();
        y *= detail::retinaScalar();
        auto centralPoint = m_animat->getCentralPoint();
        auto & pos = centralPoint.first;
        auto cx = pos.m_vec[0];
        auto cy = pos.m_vec[1];
        double sx, sy;
        detail::worldToScreen(cx, cy, sx, sy);
        auto diffX = (sx - x - centerX);
        auto diffY = centerY - (sy - y);
        auto diffXSq = std::sqrt(diffX * diffX) * viewDistance;
        auto diffYSq = std::sqrt(diffY * diffY) * viewDistance;

        if (diffXSq < 5 && diffYSq < 5) {
            *m_highlighted = true;
            // m_threadRunner.add([this]{fadeIn();});
            // m_threadRunner.add([this]{fadeOut();});
        } else {
            *m_highlighted = false;
        }
    }

    std::shared_ptr<model::Animat> GLAnimat::animatRef() 
    {
        return m_animat;
    }

    void GLAnimat::drawBody()
    {
        detail::lineWidth(4.0);
        auto & physicsEngine = m_animat->getPhysicsEngine();

        auto blocks = m_animat->getBlockCount();

        for(int b = 0; b < m_animat->getBlockCount(); ++b) {
            auto block = m_animat->getBlock(b);
            auto layer1 = block.getLayerOne();
            auto layer2 = block.getLayerTwo();
            auto layer1Left = layer1.getPositionLeft(physicsEngine);
            auto layer1Right = layer1.getPositionRight(physicsEngine);
            auto layer2Left = layer2.getPositionLeft(physicsEngine);
            auto layer2Right = layer2.getPositionRight(physicsEngine);

            detail::setColor(m_basicColor);
            glBegin(GL_LINES);
                glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], 0);
                glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], 0);
                glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], 0);
                glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], 0);
                glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], 0);
                glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], 0);
                glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], 0);
                glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], 0);
            glEnd();
            auto sc = m_animat->getSpeciesColour();
            detail::setColor({sc.R, sc.G, sc.B});
            glBegin(GL_QUADS);
                glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], 0);
                glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], 0);
                glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], 0);
                glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], 0);
            glEnd();
        }
        detail::setColor(m_basicColor);
        detail::lineWidth(1.0);
    }

    void GLAnimat::drawAntennae()
    {
        detail::lineWidth(2.0);
        auto & physicsEngine = m_animat->getPhysicsEngine();
        auto blocks = m_animat->getBlockCount();

        // Draw antennae
        auto leftAnt = m_animat->getLeftAntennaePoint();
        auto rightAnt = m_animat->getRightAntennaePoint();
        auto & layer1 = m_animat->getBlock(blocks-1).getLayerTwo();
        auto layer1Left = layer1.getPositionLeft(physicsEngine);
        auto layer1Right = layer1.getPositionRight(physicsEngine);
        glBegin(GL_LINES);
            glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], 0);
            glVertex3f(leftAnt.m_vec[0], leftAnt.m_vec[1], 0);
            glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], 0);
            glVertex3f(rightAnt.m_vec[0], rightAnt.m_vec[1], 0);
        glEnd();

        // Draw 'bobbles' on the end of each antenna
        detail::setColor(m_antennaeColor); 
        detail::drawCircle(leftAnt.m_vec[0], leftAnt.m_vec[1], 0.2, 10, true /* filled */);
        detail::drawCircle(rightAnt.m_vec[0], rightAnt.m_vec[1], 0.2, 10, true);

        // Outline
        detail::setColor(m_basicColor);
        detail::drawCircle(leftAnt.m_vec[0], leftAnt.m_vec[1], 0.2, 10);
        detail::drawCircle(rightAnt.m_vec[0], rightAnt.m_vec[1], 0.2, 10);
        detail::lineWidth(1.0);
    }

    void GLAnimat::drawBoundingCircles()
    {
        for(int b = 0; b < m_animat->getBlockCount(); ++b) {
            auto boundingPair = m_animat->getBoundingCircle(b);
            auto & centerPoint = boundingPair.first;
            detail::drawCircle(centerPoint.m_vec[0], 
                               centerPoint.m_vec[1],
                               boundingPair.second, 20);
        }
    }

    void GLAnimat::fadeIn()
    {
        auto const inc = (0.9 / 30);
        for(int i = 0; i < 30; ++i) {
            auto val = m_opacity->load();
            val += inc;
            m_opacity->store(val);
            ::usleep(10000);
        }
    }

    void GLAnimat::fadeOut()
    {
        auto const inc = (0.9 / 30);
        for(int i = 0; i < 30; ++i) {
            auto val = m_opacity->load();
            val -= inc;
            m_opacity->store(val);
            ::usleep(10000);
        }
    }

    void GLAnimat::showPopNumber(std::pair<physics::Vector3, double> 
                                 const & boundingPair)
    {
        // display population number above agent
        glPushMatrix();
        glLoadIdentity();
        detail::setColor({70, 70, 70}, *m_opacity);
        auto cx = boundingPair.first.m_vec[0];
        auto cy = boundingPair.first.m_vec[1];
        cy += (boundingPair.second * 1.2);
        double sx, sy;
        detail::worldToScreen(cx, cy, sx, sy);
        std::stringstream ss;
        ss << "ID: " << m_animat->getID();
        glfreetype::print(g_animatText, sx - 20, sy, ss.str());
        glPopMatrix();
    }

    void GLAnimat::drawBigBoundingCircle()
    {
        auto boundingPair = m_animat->getCentralPoint();
        if(*m_selected) {
            detail::lineWidth(2.0);
        } else {
            showPopNumber(boundingPair);
        }
        detail::setColor(m_bigCircleColor);
        detail::drawCircle(boundingPair.first.m_vec[0], 
                           boundingPair.first.m_vec[1],
                           boundingPair.second, 20);
        detail::setColor(m_basicColor);
        detail::lineWidth(1.0);
    }   
}