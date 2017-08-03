/// Copyright (c) 2017 Ben Jones

#include "graphics/GLCircularDial.hpp"
#include <cmath>
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

    GLCircularDial::GLCircularDial(GLFWwindow * window,
                   int xLocation,
                   int yLocation,
                   double radius,
                   int const startAngle,
                   detail::ThreadRunner & threadRunner)
      : m_window(window) 
      , m_xLocation(xLocation)
      , m_yLocation(yLocation)
      , m_threadRunner(threadRunner)
      , m_radius(radius)
      , m_entered(false)
      , m_opacity(0.3)
      , m_angle(startAngle)
      , m_state(false)
      , m_oldX(-1)
      , m_oldY(-1)
    {
    }

    void GLCircularDial::draw()
    {
        int windowWidth;
        glfwGetWindowSize(m_window, &windowWidth, &m_windowHeight);
        m_derivedX = deriveX(windowWidth, m_xLocation);
        m_derivedY = deriveY(m_windowHeight, m_yLocation);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(m_derivedX, m_derivedY, 0);
        glRotatef(m_angle, 0.0, 0.0, 1.0);

        // Outer dial circle
        glLineWidth(3.0);
        detail::setColor({10, 10, 150}, 0.3 /* opacity */);
        glBegin(GL_LINE_LOOP);
            for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                glVertex2f(::sin(angle) * m_radius, ::cos(angle) * m_radius);
            }
        glEnd();

        // Inner fill
        detail::setColor({130, 130, 150}, 0.3 /* opacity */);
        glBegin(GL_POLYGON);
            for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                glVertex2f(::sin(angle) * (m_radius - 1.0), ::cos(angle) * (m_radius - 1.0));
            }
        glEnd();

        // Inner circular element
        detail::setColor({10, 10, 150}, 0.3 /* opacity */);
        glBegin(GL_POLYGON);
            for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                glVertex2f(::sin(angle) * (m_radius / 5.0), ::cos(angle) * (m_radius / 5.0));
            }
        glEnd();

        // Dial pointer Pointer
        glLineWidth(3.0);
        detail::setColor({50, 50, 100}, 0.3 /* opacity */);
        glBegin(GL_LINES);
            glVertex2f(0, 3.0);
            glVertex2f(0, m_radius + 4.0);
        glEnd();
        glPopMatrix();
    }

    /// fade in the button when hovering over
    void GLCircularDial::fadeIn()
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
    void GLCircularDial::fadeOut()
    {
        auto const inc = (0.9 / 30);
        for(int i = 0; i < 30; ++i) {
            auto val = m_opacity.load();
            val -= inc;
            m_opacity.store(val);
            usleep(10000);
        }
    }

    void GLCircularDial::mouseIsOver(int const x, int const y)
    {    
        // initialize old x and y
        if(m_oldX == -1) {
            m_oldX = x;
        }
        if(m_oldY == -1) {
            m_oldY = y;
        }

        if (x >= m_xLocation - m_radius && 
            x <= m_xLocation + m_radius &&
            y <= (m_windowHeight - m_yLocation) + m_radius &&
            y >= (m_windowHeight - m_yLocation) - (m_radius)) {
            m_entered = true;
        } else {
            m_entered = false;
        }
        if(m_state) {

            auto distx = std::sqrt((m_xLocation - x) * (m_xLocation - x));
            auto disty = std::sqrt((m_windowHeight - m_yLocation - y) * (m_windowHeight - m_yLocation - y));
            auto angle = std::atan2(disty, distx) * 57.2985;
            std::cout<<angle<<std::endl;

            // if(x - m_oldX > 0) {
            //     if (y >= (m_windowHeight - m_yLocation)) {
            //         m_angle += 2;
            //     } else {
            //         m_angle -= 2;
            //     }
            // } else {
            //     if (y >= (m_windowHeight - m_yLocation)) {
            //         m_angle -= 2;
            //     } else {
            //         m_angle += 2;
            //     }
            // }
                // if(y - m_oldY > 0) {
                //     if (x >= m_xLocation &&
                //         x < m_xLocation + (m_radius * 2)) {
                //         m_angle -= 2;
                //     } else {
                //         m_angle += 2;
                //     }
                // } else {
                //     if (x >= m_xLocation &&
                //         x < m_xLocation + (m_radius * 2)) {
                //         m_angle += 2;
                //     } else {
                //         m_angle -= 2;
                //     }
                // }
            
        }
        m_oldX = x;
        m_oldY = y;
    }

    void GLCircularDial::handleClick(int const action)
    {
        if(action == GLFW_PRESS && m_entered) {
            m_state = true;
        } else {
            m_state = false;
        }
    }

    // void GLCircularDial::installHandler(std::function<void(bool const)> const & handler)
    // {
    //     m_handler = handler;
    // }

    void GLCircularDial::setColor(Color const & color) 
    {
        m_buttonColor = color;
    }
}