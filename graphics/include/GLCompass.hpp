/// Copyright (c) Ben Jones

#pragma once

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
            glColor4f(0.8,0.9,0.8, 1.0);

            glPushMatrix();

            glLoadIdentity();
            glTranslatef(m_windowWidth / 2.5, m_windowHeight / 2.5, 0);
            glRotatef(m_angle, 0.0, 0.0, 1.0);
            auto const rad = 50.0;

            glColor4f(1,0.5,0.5,1.0);
            glBegin(GL_LINE_LOOP);
                for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                    glVertex2f(::sin(angle) * rad, ::cos(angle) * rad);
                }
            glEnd();

            glBegin(GL_POLYGON);
                glVertex3f(0, 50, 0);
                glVertex3f(-5, 0,0);
                glVertex3f(5, 0,0);
            glEnd();
            glPopMatrix();

            glColor4f(87 / 255.0, 89 / 255.0, 92 / 255.0, 1);
        }

      private:
        double & m_angle;
        int & m_windowWidth;
        int & m_windowHeight;
    };


}