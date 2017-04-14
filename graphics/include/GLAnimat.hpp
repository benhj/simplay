// Copyright (c) 2017 Ben Jones

#pragma once

#include "PhysicsEngine.hpp"
#include "Animat.hpp"
#include <OpenGL/gl.h>
#include <cmath>

namespace graphics {

    namespace detail {

        inline void drawCircle(float cx, float cy, float r, int num_segments) 
        {
            glBegin(GL_LINE_LOOP);
            for (int ii = 0; ii < num_segments; ii++)   {
                float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
                float x = r * cosf(theta);//calculate the x component 
                float y = r * sinf(theta);//calculate the y component 
                glVertex3f(x + cx, y + cy, -10.0);//output vertex 
            }
            glEnd();
        }
    }

    struct GLAnimat
    {

        explicit GLAnimat(model::Animat & animat)
          : m_animat(animat) 
        {
        }

        GLAnimat() = delete;

        void draw()
        {
            drawBody();
            drawAntennae();
        }

      private:

        model::Animat & m_animat;

        void drawBody()
        {
            auto & physicsEngine = m_animat.getPhysicsEngine();
            for(int b = 0; b < m_animat.getBlockCount(); ++b) {
                auto block = m_animat.getBlock(b);
                auto layer1 = block.getLayerOne();
                auto layer2 = block.getLayerTwo();
                auto layer1Left = layer1.getPositionLeft(physicsEngine);
                auto layer1Right = layer1.getPositionRight(physicsEngine);
                auto layer2Left = layer2.getPositionLeft(physicsEngine);
                auto layer2Right = layer2.getPositionRight(physicsEngine);

                glBegin(GL_LINES);
                    glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
                    glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
                    glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], -10.0);
                    glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], -10.0);
                    glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
                    glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], -10.0);
                    glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
                    glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], -10.0);
                glEnd();
            }
        }

        void drawAntennae()
        {
            auto & physicsEngine = m_animat.getPhysicsEngine();
            auto blocks = m_animat.getBlockCount();
            // Draw antennae
            auto leftAnt = m_animat.getLeftAntennaePoint();
            auto rightAnt = m_animat.getRightAntennaePoint();
            auto layer1 = m_animat.getBlock(blocks-1).getLayerTwo();
            auto layer1Left = layer1.getPositionLeft(physicsEngine);
            auto layer1Right = layer1.getPositionRight(physicsEngine);
            glBegin(GL_LINES);
                glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
                glVertex3f(leftAnt.m_vec[0], leftAnt.m_vec[1], -10.0);
                glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
                glVertex3f(rightAnt.m_vec[0], rightAnt.m_vec[1], -10.0);
            glEnd();

            // Draw 'bobbles' on the end of each antenna
            detail::drawCircle(leftAnt.m_vec[0], leftAnt.m_vec[1], 0.05, 10);
            detail::drawCircle(rightAnt.m_vec[0], rightAnt.m_vec[1], 0.05, 10);
        }   
    };
}