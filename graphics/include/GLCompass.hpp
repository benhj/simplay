/// Copyright (c) Ben Jones

#pragma once

#include "Color.hpp"
#include <OpenGL/gl.h>
#include <cmath>

namespace graphics {

    class GLCompass
    {
      public:
        GLCompass(double & angle,
                  int & windowWidth,
                  int & windowHeight)
          : m_angle(angle)
          , m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
        {
        }
        GLCompass() = delete;

        void setCompassScene()
        {
            glViewport(0, 0, m_windowWidth, m_windowHeight);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho((-m_windowWidth/2) * 1, 
                    (m_windowWidth/2) * 1, 
                    (-m_windowHeight/2)* 1, 
                    (m_windowHeight/2) * 1, 
                    -10000, 10000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }

        void draw()
        {
            setCompassScene();
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(m_windowWidth / 2.5, m_windowHeight / 2.5, 0);
            glRotatef(m_angle, 0.0, 0.0, 1.0);
            auto const rad = 50.0;

            // Draw outer compass circle
            detail::setColor(m_compassOutline, 0.5 /* opacity */);
            glLineWidth(3.0);
            glBegin(GL_LINE_LOOP);
                for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                    glVertex2f(::sin(angle) * rad, ::cos(angle) * rad);
                }
            glEnd();
            glLineWidth(1.0);

            // Draw blue 'north' part of pointer
            detail::setColor(m_pointerUp);
            glBegin(GL_POLYGON);
                glVertex3f(0, 50, 0);
                glVertex3f(-5, 0,0);
                glVertex3f(5, 0,0);
            glEnd();

            // Draw white 'south' part of pointer
            detail::setColor(m_pointerDown);
            glBegin(GL_POLYGON);
                glVertex3f(0, -50, 0);
                glVertex3f(-5, 0,0);
                glVertex3f(5, 0,0);
            glEnd();

            // Draw outline around pointer
            detail::setColor(m_pointerOutline);
            glBegin(GL_LINES);
                glVertex3f(0, 50, 0);
                glVertex3f(-5, 0,0);
                glVertex3f(0, 50, 0);
                glVertex3f(5, 0,0);
                glVertex3f(0, -50, 0);
                glVertex3f(-5, 0,0);
                glVertex3f(0, -50, 0);
                glVertex3f(5, 0,0);
            glEnd();
            glPopMatrix();

            glColor4f(87 / 255.0, 89 / 255.0, 92 / 255.0, 1);
        }

      private:
        double & m_angle;
        int & m_windowWidth;
        int & m_windowHeight;
        Color m_pointerUp { 0, 0, 255 };
        Color m_pointerDown { 255, 255, 255};
        Color m_pointerOutline { 0, 0, 0 };
        Color m_compassOutline { 100, 100, 100 };
    };


}