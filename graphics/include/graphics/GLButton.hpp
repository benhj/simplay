/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "ThreadRunner.hpp"
#include <OpenGL/gl.h>
#include <atomic>
#include <cmath>
#include <functional>
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
          , m_handler()
          , m_state(false)
        {
        }
        GLButton() = delete;

        void draw()
        {
            glTranslatef(m_derivedX, m_derivedY, 0);
            detail::setColor(m_buttonColor, m_opacity /* opacity */);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, 50);
                glVertex2f(0, 0);
                glVertex2f(80, 50);
                glVertex2f(80, 50);
                glVertex2f(0, 0);
                glVertex2f(80, 0);
            glEnd();

            glLineWidth(3.0);
            detail::setColor({50, 50, 50}, m_opacity /* opacity */);
            glBegin(GL_LINE_LOOP);
                glVertex2f(0, 50);
                glVertex2f(0, 0);
                glVertex2f(80, 0);
                glVertex2f(80, 50);
            glEnd();
        }

        /// fade in the button when hovering over
        void fadeIn()
        {
            auto const inc = (1.0 / 30);
            for(int i = 0; i < 30; ++i) {
                auto val = m_opacity.load();
                val += inc;
                m_opacity.store(val);
                usleep(10000);
            }
        }

        /// fade out the button on pointer exit
        void fadeOut()
        {
            auto const inc = (1.0 / 30);
            for(int i = 0; i < 30; ++i) {
                auto val = m_opacity.load();
                val -= inc;
                m_opacity.store(val);
                usleep(10000);
            }
        }

        /// When pointer over button, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        bool mouseIsOver(int const x, int const y)
        {    
            if (x >= m_xLocation && 
                x <= m_xLocation + 80 &&
                y <= m_yLocation &&
                y >= m_yLocation - 50) {
                if(!m_entered.exchange(true)) {
                    m_threadRunner.add([this]{fadeIn();});
                }
            } else {
                if(m_entered.exchange(false)) {
                    m_threadRunner.add([this]{fadeOut();});
                }
            }
        }

        void handleClick() 
        {
            if(m_entered && m_handler) {
                m_state = !m_state;
                m_handler(m_state);
            }
        }

        void installHandler(std::function<void(bool const)> const & handler)
        {
            m_handler = handler;
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of button
        std::atomic<double> m_opacity;

        /// Callback to trigger on click
        std::function<void(bool const)> m_handler;

        // Default colour
        Color m_buttonColor { 150, 200, 150 };
    };


}