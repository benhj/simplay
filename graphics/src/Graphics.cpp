// Copyright (c) 2017 Ben Jones

#include "Graphics.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

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
    Graphics::Graphics(GLEnvironment & glEnviro)
      : m_glEnviro(glEnviro)
      , m_viewDistance(0.4)
    {
        init();
    }

    void Graphics::display()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_glEnviro.draw();
        glutSwapBuffers();
    }

    void Graphics::reshape(int const w, int const h)
    {
        m_glEnviro.updateWidthHeight(w, h);
    }

    void Graphics::passiveMouseFunc(int const x, int const y)
    {
        m_glEnviro.checkForAnimatHighlight(x, y);
    }

    void Graphics::keyboardHandler(int const key, 
                                   int const x, 
                                   int const y)
    {
        // zoom control
        if (key == '+') {
            auto loaded = m_viewDistance.load();
            loaded -= 0.01;
            m_viewDistance = loaded;
            m_glEnviro.setViewDistance(loaded);
        } else if (key == '-') {
            auto loaded = m_viewDistance.load();
            loaded += 0.01;
            m_viewDistance = loaded;
            m_glEnviro.setViewDistance(loaded);
        } 
        // centre axis on/off
        else if (key == 'a') {
            m_glEnviro.toggleAxisDisplay();
        } 
        // world rotation control
        else if (key == 102) /* right arrow */{
            m_glEnviro.spinRight();    
        } else if (key == 100) /* left arrow */{
            m_glEnviro.spinLeft(); 
        }
    }
}

