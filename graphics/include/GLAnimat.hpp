// Copyright (c) 2017 Ben Jones

#pragma once

#include "PhysicsEngine.hpp"
#include "Animat.hpp"
#include <OpenGL/gl.h>
#include <cmath>
#include <atomic>
#include <memory>

namespace graphics {

    struct Color {
        double R;
        double G;
        double B;
    };

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
                float x = r * cosf(theta);//calculate the x component 
                float y = r * sinf(theta);//calculate the y component 
                glVertex3f(x + cx, y + cy, 0);//output vertex 
            }
            glEnd();
        }

        inline void setColor(Color const & color)
        {
            glColor4f(color.R / 255.0, color.G / 255.0, color.B / 255.0, 1.0);
        }
    }

    struct GLAnimat
    {

        explicit GLAnimat(model::Animat & animat)
          : m_animat(animat)
          , m_highlighted(std::make_shared<std::atomic<bool>>(false)) 
        {
        }

        GLAnimat() = delete;

        void draw()
        {
            detail::setColor(m_basicColor);
            drawBody();
            drawAntennae();
            drawBoundingCircles();
            if (*m_highlighted) {
                drawBigBoundingCircle();
            }
        }

        void highlight()
        {
            *m_highlighted = true;
        }

        void dehighlight()
        {
            *m_highlighted = false;
        }

      private:

        model::Animat & m_animat;

        Color m_basicColor { 87, 89, 92 };
        Color m_segmentColor { 197, 217, 200 };
        Color m_antennaeColor { 222, 153, 153 };

        /// If mouse pointer over animat, draw big bounding circle around it
        /// to indicate it's 'selected'
        std::shared_ptr<std::atomic<bool>> m_highlighted;

        void drawBody()
        {
            glLineWidth(5.0);
            auto & physicsEngine = m_animat.getPhysicsEngine();
            for(int b = 0; b < m_animat.getBlockCount(); ++b) {
                auto block = m_animat.getBlock(b);
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
                detail::setColor(m_segmentColor);
                glBegin(GL_QUADS);
                    glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], 0);
                    glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], 0);
                    glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], 0);
                    glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], 0);
                glEnd();
            }
            detail::setColor(m_basicColor);
            glLineWidth(1.0);
        }

        void drawAntennae()
        {
            glLineWidth(3.0);
            auto & physicsEngine = m_animat.getPhysicsEngine();
            auto blocks = m_animat.getBlockCount();
            // Draw antennae
            auto leftAnt = m_animat.getLeftAntennaePoint();
            auto rightAnt = m_animat.getRightAntennaePoint();
            auto layer1 = m_animat.getBlock(blocks-1).getLayerTwo();
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
            detail::drawCircle(leftAnt.m_vec[0], leftAnt.m_vec[1], 0.5, 5, true /* filled */);
            detail::drawCircle(rightAnt.m_vec[0], rightAnt.m_vec[1], 0.5, 5, true);

            // Outline
            detail::setColor(m_basicColor);
            detail::drawCircle(leftAnt.m_vec[0], leftAnt.m_vec[1], 0.5, 5);
            detail::drawCircle(rightAnt.m_vec[0], rightAnt.m_vec[1], 0.5, 5);
            glLineWidth(1.0);
        }

        void drawBoundingCircles()
        {
            for(int b = 0; b < m_animat.getBlockCount(); ++b) {
                auto boundingPair = m_animat.getBoundingCircle(b);
                auto & centerPoint = boundingPair.first;
                detail::drawCircle(centerPoint.m_vec[0], 
                                   centerPoint.m_vec[1],
                                   boundingPair.second, 20);
            }
        }

        void drawBigBoundingCircle()
        {
            auto boundingPair = m_animat.getCentralPoint();
            detail::drawCircle(boundingPair.first.m_vec[0], 
                               boundingPair.first.m_vec[1],
                               boundingPair.second, 20);
        }   
    };
}