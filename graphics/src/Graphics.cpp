// Copyright (c) 2017 Ben Jones

#include "graphics/Graphics.hpp"
#include "graphics/SetScene.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>

namespace {
    void init() // Called before main loop to set up the program
    {
        glClearColor(209.0 / 255.0, 
                    220.0 / 255.0, 
                    235.0 / 255.0, 0.0);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glShadeModel(GL_SMOOTH);
    }
}

namespace graphics {
    Graphics::Graphics(int & windowWidth,
                       int & windowHeight,
                       GLEnvironment & glEnviro)
      : m_windowWidth(windowWidth)
      , m_windowHeight(windowHeight)
      , m_glEnviro(glEnviro)
      , m_viewDistance(0.4)
      , m_testButton(windowWidth, 
                     windowHeight,
                     20, windowHeight-20)
    {
        init();
    }

    void Graphics::display()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_glEnviro.draw();

        // Draw GUI elements here
        drawGUIElements();
    }

    void Graphics::reshape(int const w, int const h)
    {
        m_windowWidth = w;
        m_windowHeight = h;
        m_glEnviro.updateWidthHeight(w, h);
    }

    void Graphics::passiveMouseFunc(double const x, double const y)
    {
        m_glEnviro.checkForAnimatHighlight(x, y);
        m_testButton.mouseIsOver(x, y);
    }

    void Graphics::keyboardHandler(int const key, 
                                   int const scancode, 
                                   int const action, 
                                   int const mods) 
    {
        if (action == GLFW_PRESS) {
            handleKeyDown(key);
        } else if(action == GLFW_RELEASE) {
            handleKeyUp(key);
        } else if(action == GLFW_REPEAT) {
            handleKeyContinuous(key);
        }
    }

    void Graphics::handleKeyContinuous(int const key)
    {
        // Don't allow continuous axis toggling
        if (key != GLFW_KEY_A) {
            handleKeyDown(key);
        }
    }

    void Graphics::handleKeyDown(int const key)
    {
        // zoom control
        if (key == '+' || key == '=') {
            auto loaded = m_viewDistance.load();
            loaded -= 0.01;
            m_viewDistance = loaded;
            m_glEnviro.setViewDistance(loaded);
        } else if (key == GLFW_KEY_MINUS) {
            auto loaded = m_viewDistance.load();
            loaded += 0.01;
            m_viewDistance = loaded;
            m_glEnviro.setViewDistance(loaded);
        } 
        // centre axis on/off; 
        // note always code 65 ('A') even for lowercase
        else if (key == GLFW_KEY_A) {
            m_glEnviro.toggleAxisDisplay();
        } 
        // world rotation control
        else if (key == GLFW_KEY_RIGHT) /* right arrow */{
            m_glEnviro.compassOn();
            m_glEnviro.spinRight();    
        } else if (key == GLFW_KEY_LEFT) /* left arrow */{
            m_glEnviro.compassOn();
            m_glEnviro.spinLeft(); 
        } else if (key == GLFW_KEY_SPACE) {
            m_glEnviro.flyIn();
        }
    }

    void Graphics::handleKeyUp(int const)
    {
        // Currently the only thing useful that can
        // happen here is disabling the compass
        m_glEnviro.compassOff();
    }

    void Graphics::clickHandler(int const button, 
                                int const action, 
                                int const mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            m_glEnviro.selectAnimat();
        }
    }

    void Graphics::drawGUIElements()
    {
        drawGUIElementsSetup();
        m_testButton.draw();
        drawGUIElementsTearDown();

    }

    void Graphics::drawGUIElementsSetup()
    {
        detail::setScene(m_windowWidth, m_windowHeight, 1, 0);
        glPushMatrix();
        glLoadIdentity();
    }

    void Graphics::drawGUIElementsTearDown()
    {
        detail::setScene(m_windowWidth, 
                         m_windowHeight, 
                         m_glEnviro.getViewDistance(), 
                         m_glEnviro.getWorldOrientation());
    }

}

