// Copyright (c) 2017 - present Ben Jones

#include "graphics/Graphics.hpp"
#include "graphics/SetScene.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>

namespace {
    void init() // Called before main loop to set up the program
    {
        glClearColor(230.0 / 255.0, 
                    230.0 / 255.0, 
                    250.0 / 255.0, 0.0);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_MULTISAMPLE);  
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
}

namespace graphics {
    Graphics::Graphics(int & windowWidth,
                       int & windowHeight,
                       GLEnvironment & glEnviro,
                       detail::ThreadRunner & threadRunner)
      : m_windowWidth(windowWidth)
      , m_windowHeight(windowHeight)
      , m_glEnviro(glEnviro)
      , m_threadRunner(threadRunner)
      , m_viewDistance(0.4)
      , m_guiElements()
      , m_console(m_windowWidth,       // width
                  m_windowHeight / 4,  // height
                  0,                   // x 
                  m_windowHeight - 50, // y
                  12,                  // font size
                  "/Library/Fonts/Courier New.ttf")
      , m_displayConsole(false)
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
        for (auto & b : m_guiElements) {
            b->mouseIsOver(x, y);
        }
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
        if (action == GLFW_PRESS) {
            m_glEnviro.selectAnimat();

        }
        for(auto & b : m_guiElements) {
            b->handleClick(action);
        }
    }

    void Graphics::drawGUIElements()
    {
        drawGUIElementsSetup();
        for(auto & b : m_guiElements) {
            b->draw();
        }

        // Draw the console, if enabled
        if(m_displayConsole.load()) {
            m_console.display();
        }

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
        glPopMatrix();
    }

    void Graphics::addGUIElement(std::shared_ptr<GLGUIElement> element)
    {
        m_guiElements.emplace_back(std::move(element));
    }

}

