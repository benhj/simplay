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


// namespace {
//     inline int deriveX(int const windowWidth, int const xLocation)
//     {
//         return -(windowWidth / 2) + xLocation;
//     }
//     inline int deriveY(int const windowHeight, int const yLocation)
//     {
//         return -(windowHeight / 2) + yLocation;
//     }
// }

namespace graphics {

    class GLVerticalSlider
    {
      public:
        GLVerticalSlider(GLFWwindow * window,
                         int windowOffset,
                         int width,
                         detail::ThreadRunner & threadRunner)
          : m_window(window) 
          , m_windowOffset(windowOffset)
          , m_width(width)
          , m_threadRunner(threadRunner)
          // , m_width(width)
          // , m_height(height)
          , m_entered(false)
          , m_opacity(0.3)
          // , m_handler()
          // , m_overLay()
          // , m_state(false)
        {
        }
        GLVerticalSlider() = delete;

        void draw()
        {
            glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
            m_derivedX = (m_windowWidth / 2) - m_windowOffset;
            //std::cout<<windowW<<std::endl;
            m_derivedY = -(m_windowHeight / 2) + m_windowOffset;
            auto length = m_windowHeight - (m_windowOffset * 2);
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(m_derivedX, m_derivedY, 0);
            detail::setColor(m_buttonColor, m_opacity);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, length);
                glVertex2f(0, 0);
                glVertex2f(m_width, length);
                glVertex2f(m_width, length);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
            glEnd();

            glLineWidth(3.0);
            detail::setColor({150, 150, 150}, m_opacity);
            glBegin(GL_LINE_LOOP);
                glVertex2f(0, length);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
                glVertex2f(m_width, length);
            glEnd();
            glPopMatrix();
        }

        /// fade in the element when hovering over
        void fadeIn()
        {
            auto const inc = (0.7 / 30);
            for(int i = 0; i < 30; ++i) {
                auto val = m_opacity.load();
                val += inc;
                m_opacity.store(val);
                usleep(10000);
            }
        }

        /// fade out the element on pointer exit
        void fadeOut()
        {
            auto const inc = (0.7 / 30);
            for(int i = 0; i < 30; ++i) {
                auto val = m_opacity.load();
                val -= inc;
                m_opacity.store(val);
                usleep(10000);
            }
        }

        /// When pointer over element, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        bool mouseIsOver(int const x, int const y)
        {    
            if (x >= m_windowWidth - m_windowOffset &&
                x < m_windowWidth - m_windowOffset + m_width &&
                y < m_windowHeight - m_windowOffset &&
                y >= m_windowOffset) {
                if(!m_entered.exchange(true)) {
                    m_threadRunner.add([this]{fadeIn();});
                }
            } else {
                if(m_entered.exchange(false)) {
                    m_threadRunner.add([this]{fadeOut();});
                }
            }
        }

        // void handleClick() 
        // {
        //     if(m_entered && m_handler) {
        //         m_state = !m_state;
        //         m_handler(m_state);
        //     }
        // }

        // void installHandler(std::function<void(bool const)> const & handler)
        // {
        //     m_handler = handler;
        // }

        // void setOverlay(std::function<void(bool const)> const & overlay)
        // {
        //     m_overLay = overlay;
        // }

        // void setColor(Color const & color) 
        // {
        //     m_buttonColor = color;
        // }

      private:
        GLFWwindow* m_window;
        int m_derivedX;
        int m_derivedY;
        int m_windowWidth;
        int m_windowHeight;
        int m_windowOffset;
        int m_width;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Slider length
        //int m_length;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        // /// True when mouse over, false otherwise
        // std::atomic<bool> m_state;

        /// For fade-level of element
        std::atomic<double> m_opacity;

        // /// Callback to trigger on click
        // std::function<void(bool const)> m_handler;

        // /// For drawing a simple overlay
        // std::function<void(bool const)> m_overLay;

        // Default colour
        Color m_buttonColor { 155, 155, 155 };
    };


}