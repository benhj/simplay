/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "SetScene.hpp"
#include "ThreadRunner.hpp"
#include <OpenGL/gl.h>
#include <atomic>
#include <cmath>
#include <future>
#include <unistd.h>

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
                 int yLocation,
                 detail::ThreadRunner & threadRunner)
          : m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
          , m_xLocation(xLocation)
          , m_yLocation(yLocation)
          , m_derivedX(deriveX(windowWidth, xLocation))
          , m_derivedY(deriveY(windowHeight, yLocation))
          , m_threadRunner(threadRunner)
          , m_entered(false)
          , m_opacity(0)
        {
        }
        GLButton() = delete;

        void draw()
        {
            glTranslatef(m_derivedX, m_derivedY, 0);

            // Draw outer compass circle
            detail::setColor(m_buttonColor, m_opacity /* opacity */);
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
            auto const inc = 1.0 / 30;
            for(int i = 0; i < 30; ++i) {
                auto val = m_opacity.load();
                val += inc;
                m_opacity.store(val);
                usleep(10000);
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
                if(!m_entered.exchange(true)) {
                    m_threadRunner.add([this]{fadeIn();});
                }
            }
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;
        detail::ThreadRunner & m_threadRunner;
        std::atomic<bool> m_entered;
        std::atomic<double> m_opacity;
        Color m_buttonColor { 150, 150, 150 };
    };


}