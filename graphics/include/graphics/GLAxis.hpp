// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "SetScene.hpp"
#include <atomic>
#include <OpenGL/gl.h>

namespace graphics {
    class GLAxis
    {
      public:
        GLAxis(int & windowWidth,
               int & windowHeight,
               std::atomic<double> & viewDistance,
               std::atomic<double> & worldOrientation)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_viewDistance(viewDistance)
        , m_worldOrientation(worldOrientation)
        {
        }

        void draw()
        {
            detail::setScene(m_windowWidth, 
                             m_windowHeight,
                             0,
                             m_worldOrientation);
            detail::setColor(m_axisColor);
            glBegin(GL_LINES);
            glVertex3f(0, -(m_windowHeight/2), 0);
            glVertex3f(0, (m_windowHeight/2), 0);
            glVertex3f(-(m_windowWidth/2), 0, 0);
            glVertex3f((m_windowWidth/2), 0, 0);
            glEnd();
            detail::setScene(m_windowWidth, 
                             m_windowHeight,
                             m_viewDistance,
                             m_worldOrientation);
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        std::atomic<double> & m_viewDistance;
        std::atomic<double> & m_worldOrientation;
        Color m_axisColor { 87, 79, 92 };
    };
}