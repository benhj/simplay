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
    {
    }

    void GLCircularDial::draw()
    {
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
        m_derivedX = deriveX(windowWidth, m_xLocation);
        m_derivedY = deriveY(windowHeight, m_yLocation);
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

        // Inner circular element
        glLineWidth(1.0);
        detail::setColor({10, 10, 150}, 0.3 /* opacity */);
        glBegin(GL_LINE_LOOP);
            for (double angle = 0; angle < 3.1415926f * 2; angle += 0.25) {
                glVertex2f(::sin(angle) * (m_radius / 5.0), ::cos(angle) * (m_radius / 5.0));
            }
        glEnd();

        // glBegin(GL_LINE)


        // glBegin(GL_TRIANGLES);
        //     glVertex2f(0, m_height);
        //     glVertex2f(0, 0);
        //     glVertex2f(m_width, m_height);
        //     glVertex2f(m_width, m_height);
        //     glVertex2f(0, 0);
        //     glVertex2f(m_width, 0);
        // glEnd();

        // if(m_overLay) {
        //     detail::setColor({250, 250, 250}, m_opacity /* opacity */);
        //     m_overLay(m_state);
        // }

        
        // glBegin(GL_LINE_LOOP);
        //     glVertex2f(0, m_height);
        //     glVertex2f(0, 0);
        //     glVertex2f(m_width, 0);
        //     glVertex2f(m_width, m_height);
        // glEnd();
        glPopMatrix();
    }

    // /// fade in the button when hovering over
    // void fadeIn()
    // {
    //     auto const inc = (0.9 / 30);
    //     for(int i = 0; i < 30; ++i) {
    //         auto val = m_opacity.load();
    //         val += inc;
    //         m_opacity.store(val);
    //         usleep(10000);
    //     }
    // }

    // /// fade out the button on pointer exit
    // void fadeOut()
    // {
    //     auto const inc = (0.9 / 30);
    //     for(int i = 0; i < 30; ++i) {
    //         auto val = m_opacity.load();
    //         val -= inc;
    //         m_opacity.store(val);
    //         usleep(10000);
    //     }
    // }

    void GLCircularDial::mouseIsOver(int const x, int const y)
    {    
        // if (x >= m_xLocation && 
        //     x <= m_xLocation + m_width &&
        //     y <= (m_windowHeight - m_yLocation) &&
        //     y >= (m_windowHeight - m_yLocation) - m_height) {
        //     if(!m_entered.exchange(true)) {
        //         m_threadRunner.add([this]{fadeIn();});
        //     }
        // } else {
        //     if(m_entered.exchange(false)) {
        //         m_threadRunner.add([this]{fadeOut();});
        //     }
        // }
    }

    void GLCircularDial::handleClick(int const action)
    {
        // if(action == GLFW_PRESS && m_entered && m_handler) {
        //     m_state = !m_state;
        //     m_handler(m_state);
        // }
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