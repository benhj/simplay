/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "SetScene.hpp"
#include <OpenGL/gl.h>
#include <atomic>
#include <cmath>

namespace {
    inline int deriveX(int const windowWidth, int const xLocation)
    {
        return -(windowWidth / 2) + xLocation;
    }
    inline int deriveY(int const windowHeight, int const yLocation)
    {
        return -(windowHeight / 2) + (windowHeight - yLocation);
    }
}

namespace graphics {

    class GLButton
    {
      public:
        GLButton(int & windowWidth,
                 int & windowHeight,
                 int xLocation,
                 int yLocation)
          : m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
          , m_xLocation(xLocation)
          , m_yLocation(yLocation)
          , m_derivedX(deriveX(windowWidth, xLocation))
          , m_derivedY(deriveY(windowHeight, yLocation))
        {
        }
        GLButton() = delete;

        void draw()
        {
            // Position in bottom left of window

            //std::cout<<-(m_windowWidth / 2)<<std::endl;

            glTranslatef(m_derivedX, m_derivedY, 0);

            // Draw outer compass circle
            detail::setColor(m_buttonColor, 0.5 /* opacity */);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, 50);
                glVertex2f(0, 0);
                glVertex2f(80, 50);
                glVertex2f(80, 50);
                glVertex2f(0, 0);
                glVertex2f(80, 0);
            glEnd();
        }

        /// fade in the button when near to it
        void fadeIn()
        {
            for(int i = 0; i < 10; ++i) {
                //usleep(10000);
                m_buttonColor.R -= 25;
                m_buttonColor.G -= 25;
                m_buttonColor.B -= 25;
            }
        }

        /// Indicates if mouse pointer is hovering over button 
        bool mouseIsOver(int const x, int const y)
        {    
            // If mouse is over, fadeIn
            if (x >= m_xLocation && 
                x <= m_xLocation + 80 &&
                y <= m_yLocation &&
                y >= m_yLocation - 50) {
            }
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;
        Color m_buttonColor { 255, 255, 255 };
    };


}