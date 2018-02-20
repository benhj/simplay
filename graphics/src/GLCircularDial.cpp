/// Copyright (c) 2017 Ben Jones

#include "graphics/GLCircularDial.hpp"
#include "graphics/RetinaScalar.hpp"
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
    inline int deriveAngleFromPercentage(double percentage) 
    {
        // Scale so that zero and 100 aren't at precisely south
        percentage *= 0.8;
        percentage += 10.0;
        return static_cast<int>((100 - percentage) * 3.6) + (180);
    }
    inline int derivePercentageFromAngle(double angle)
    {
        auto percentage = (((angle - 180.0) / 3.6) - 100);
        percentage += 10;
        percentage /= 0.8;
        return -static_cast<int>(percentage);
    }
}

namespace graphics {

    GLCircularDial::GLCircularDial(GLFWwindow * window,
                   int xLocation,
                   int yLocation,
                   double radius,
                   int const startLevel,
                   detail::ThreadRunner & threadRunner)
      : m_window(window) 
      , m_xLocation(xLocation)
      , m_yLocation(yLocation)
      , m_threadRunner(threadRunner)
      , m_radius(radius)
      , m_entered(false)
      , m_opacity(0.3)
      , m_angle(deriveAngleFromPercentage(startLevel))
      , m_level(startLevel)
      , m_state(false)
      , m_handler()
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
        lineWidth(3.0);
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
        lineWidth(3.0);
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
            auto angle = std::atan2(disty, distx) * 57.2985; // radians to angle
            if(x <= m_xLocation) {
                int newAngle;
                if (y <= (m_windowHeight - m_yLocation)) {
                    newAngle = 90.0 - angle;
                } else {
                    newAngle = 90.0 + angle;
                }
                // Hacky as fuck, jeez...
                m_level = 50.0 - (175 - derivePercentageFromAngle(newAngle));
                if (m_level >= 0) {
                    m_angle = newAngle;
                }
            } else if(x > m_xLocation) {
                int newAngle;
                if (y <= (m_windowHeight - m_yLocation)) {
                    newAngle = 360 - (90.0 - angle);
                } else {
                    newAngle = 270 - (angle);
                }
                m_level = derivePercentageFromAngle(newAngle);
                if (m_level <= 100) {
                    m_angle = newAngle;
                }
            }

            if(m_handler) {
                m_handler(m_level);
            }

        }
    }

    void GLCircularDial::handleClick(int const action)
    {
        if(action == GLFW_PRESS && m_entered) {
            m_state = true;
        } else {
            m_state = false;
        }
    }

    void GLCircularDial::installHandler(std::function<void(int const)> const & handler)
    {
        m_handler = handler;
    }

    void GLCircularDial::setColor(Color const & color) 
    {
        m_buttonColor = color;
    }

    int GLCircularDial::getCurrentLevel() const
    {
        return m_level;
    }
}