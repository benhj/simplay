/// Copyright (c) 2017 Ben Jones

#pragma once

#include "GLGUIElement.hpp"
#include "Color.hpp"
#include "ThreadRunner.hpp"
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <cmath>
#include <functional>
#include <unistd.h>

namespace graphics {

    namespace {

        inline void drawCircle(float cx, float cy, float r, int num_segments, bool const filled = false) 
        {
            if (!filled) {
                glBegin(GL_LINE_LOOP);
            } else {
                glBegin(GL_POLYGON);
            }
            for (int ii = 0; ii < num_segments; ii++)   {
                float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
                float x = r * ::cosf(theta);//calculate the x component 
                float y = r * ::sinf(theta);//calculate the y component 
                glVertex3f(x + cx, y + cy, 0);//output vertex 
            }
            glEnd();
        }

        inline int getWindowHeight(GLFWwindow * window)
        {
            int width,height;
            glfwGetWindowSize(window, &width, &height);
            return height;
        }
    }

    class GLVerticalSlider : public GLGUIElement
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
          , m_entered(false)
          , m_opacity(0.3)
          , m_sliderPosition(getWindowHeight(window) / 2)
          , m_state(false)
        {
        }
        GLVerticalSlider() = delete;

        void draw() override
        {
            int winwidth, winheight;
            glfwGetWindowSize(m_window, &winwidth, &winheight);
            m_windowWidth = winwidth;
            m_windowHeight = winheight;
            m_derivedX = (m_windowWidth / 2) - m_windowOffset;
            m_derivedY = -(m_windowHeight / 2) + m_windowOffset;
            m_length = m_windowHeight - (m_windowOffset * 2);
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(m_derivedX, m_derivedY, 0);
            detail::setColor(m_buttonColor, m_opacity);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, m_length);
                glVertex2f(0, 0);
                glVertex2f(m_width, m_length);
                glVertex2f(m_width, m_length);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
            glEnd();

            glLineWidth(3.0);
            detail::setColor({10, 10, 150}, m_opacity);
            glBegin(GL_LINE_LOOP);
                glVertex2f(0, m_length);
                glVertex2f(0, 0);
                glVertex2f(m_width, 0);
                glVertex2f(m_width, m_length);
            glEnd();

            // Draw slider knob
            drawSliderKnob();

            glPopMatrix();
        }

        /// When pointer over element, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        bool mouseIsOver(int const x, int const y) override
        {    
            if (x >= m_windowWidth - m_windowOffset &&
                x < m_windowWidth - m_windowOffset + m_width &&
                y < m_windowHeight - m_windowOffset &&
                y >= m_windowOffset) {
                m_entered = true;
            } else {
                m_entered = false;
            }
            if(m_state) {
                if(y < m_windowHeight - m_windowOffset &&
                   y >= m_windowOffset) {
                    m_sliderPosition = y;
                    m_handler(m_currentVal);
                }
            }
        }

        void handleClick(int const action) override
        {
            if(action == GLFW_PRESS && m_entered) {
                m_state = true;
            } else {
                m_state = false;
            }
        }

        void installHandler(std::function<void(double const value)> const & handler)
        {
            m_handler = handler;
        }

        void setValue(double const value)
        {
            std::cout<<value<<std::endl;
            std::cout<<m_length<<std::endl;
            auto newPosition = value * static_cast<double>(m_length);
            std::cout<<newPosition<<std::endl;
            m_sliderPosition = newPosition - m_windowHeight + m_windowOffset;
            std::cout<<m_sliderPosition<<std::endl;
        }

      private:
        GLFWwindow* m_window;
        int m_derivedX;
        int m_derivedY;
        std::atomic<int> m_windowWidth;
        std::atomic<int> m_windowHeight;
        int m_windowOffset;
        int m_width;
        std::atomic<int> m_length;

        /// Position 0 is center of slider.
        /// Up in -ve, down is +ve
        int m_sliderPosition;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of element
        std::atomic<double> m_opacity;

        /// Default colour
        Color m_buttonColor { 155, 155, 155 };

        /// 'Value' of the slider (up is +ve)
        double m_currentVal;

        /// Callback to trigger some change based on slider position
        std::function<void(double const value)> m_handler;

        /// Draw slider knob at some position on slider
        void drawSliderKnob()
        {
            auto position = - m_sliderPosition 
                            + m_windowHeight
                            - m_windowOffset;

            m_currentVal = static_cast<double>(position) / 
                           static_cast<double>(m_length);

            detail::drawCircle(m_width / 2.0, 
                               position,
                               15.0, 
                               10,
                               true);

            // highlight knob when entered
            detail::setColor({10, 10, 10}, m_opacity);
            detail::drawCircle(m_width / 2.0, 
                               position,
                               15.0, 
                               10);
            
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
    };


}