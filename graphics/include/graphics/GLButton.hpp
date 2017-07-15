/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "ThreadRunner.hpp"
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
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
        return -(windowHeight / 2) + yLocation;
    }
}

namespace graphics {

    class GLButton
    {
      public:
        GLButton(GLFWwindow * window,
                 int xLocation,
                 int yLocation,
                 int width,
                 int height,
                 detail::ThreadRunner & threadRunner)
          : m_window(window) 
          , m_xLocation(xLocation)
          , m_yLocation(yLocation)
          , m_threadRunner(threadRunner)
          , m_width(width)
          , m_height(height)
          , m_entered(false)
          , m_opacity(0.1)
          , m_handler()
          , m_overLay()
          , m_state(false)
        {
        }
        GLButton() = delete;

        void draw()
        {
            int windowWidth;
            glfwGetWindowSize(m_window, &windowWidth, &m_windowHeight);
            m_derivedX = deriveX(windowWidth, m_xLocation);
            m_derivedY = deriveY(m_windowHeight, m_yLocation);
            glTranslatef(m_derivedX, m_derivedY, 0);
            detail::setColor(m_buttonColor, m_opacity /* opacity */);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, m_height);
                glVertex2f(0, 0);
                glVertex2f(m_width, m_height);
                glVertex2f(m_width, m_height);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
            glEnd();

            if(m_overLay) {
                detail::setColor({250, 250, 250}, m_opacity /* opacity */);
                m_overLay(m_state);
            }

            glLineWidth(3.0);
            detail::setColor({150, 150, 150}, m_opacity /* opacity */);
            glBegin(GL_LINE_LOOP);
                glVertex2f(0, m_height);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
                glVertex2f(m_width, m_height);
            glEnd();
        }

        /// fade in the button when hovering over
        void fadeIn()
        {
            auto const inc = (0.9 / 30);
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
            auto const inc = (0.9 / 30);
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
                x <= m_xLocation + m_width &&
                y <= (m_windowHeight - m_yLocation) &&
                y >= (m_windowHeight - m_yLocation) - m_height) {
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

        void setOverlay(std::function<void(bool const)> const & overlay)
        {
            m_overLay = overlay;
        }

        void setColor(Color const & color) 
        {
            m_buttonColor = color;
        }

      private:
        GLFWwindow* m_window;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;
        int m_windowHeight;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Button width and height
        int m_width;
        int m_height;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of button
        std::atomic<double> m_opacity;

        /// Callback to trigger on click
        std::function<void(bool const)> m_handler;

        /// For drawing a simple overlay
        std::function<void(bool const)> m_overLay;

        // Default colour
        Color m_buttonColor { 175, 175, 225 };
    };


}