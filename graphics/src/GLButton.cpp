/// Copyright (c) 2017-2018 Ben Jones

#include "graphics/GLButton.hpp"
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
}

namespace graphics {

    GLButton::GLButton(GLFWwindow * window,
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
      , m_opacity(0.3)
      , m_handler()
      , m_overLay()
      , m_state(false)
    {
    }

    void GLButton::draw()
    {
        int windowWidth;
        glfwGetWindowSize(m_window, &windowWidth, &m_windowHeight);
        m_derivedX = deriveX(windowWidth, m_xLocation);
        m_derivedY = deriveY(m_windowHeight, m_yLocation);
        glPushMatrix();
        glLoadIdentity();
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

        detail::lineWidth(3.0);
        detail::setColor({10, 10, 150}, 0.3 /* opacity */);
        glBegin(GL_LINE_LOOP);
            glVertex2f(0, m_height);
            glVertex2f(0, 0);
            glVertex2f(m_width, 0);
            glVertex2f(m_width, m_height);
        glEnd();
        glPopMatrix();
    }

    /// fade in the button when hovering over
    void GLButton::fadeIn()
    {
        auto const inc = (0.9 / 30);
        for(int i = 0; i < 30; ++i) {
            auto val = m_opacity.load();
            val += inc;
            m_opacity.store(val);
            ::usleep(10000);
        }
    }

    /// fade out the button on pointer exit
    void GLButton::fadeOut()
    {
        auto const inc = (0.9 / 30);
        for(int i = 0; i < 30; ++i) {
            auto val = m_opacity.load();
            val -= inc;
            m_opacity.store(val);
            ::usleep(10000);
        }
    }

    /// When pointer over button, a 'fade-in'
    /// is triggered, or a 'fade-out' on exit.
    void GLButton::mouseIsOver(int const x, int const y)
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

    void GLButton::handleClick(int const action)
    {
        if(action == GLFW_PRESS && m_entered && m_handler) {
            m_state = !m_state;
            m_handler(m_state);
        }
    }

    void GLButton::installHandler(std::function<void(bool const)> const & handler)
    {
        m_handler = handler;
    }

    void GLButton::setOverlay(std::function<void(bool const)> const & overlay)
    {
        m_overLay = overlay;
    }

    void GLButton::setColor(Color const & color) 
    {
        m_buttonColor = color;
    }
}