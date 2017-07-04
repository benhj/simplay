/// Copyright (c) 2017 Ben Jones

#pragma once

#include <OpenGL/gl.h>
#include <functional>

namespace graphics {

    inline std::function<void(bool const)> pauseOverlay()
    {
        return [](bool const state){

            if (!state) {
                glBegin(GL_TRIANGLES);
                    glVertex2f(30, 40);
                    glVertex2f(30, 10);
                    glVertex2f(36, 40);
                    glVertex2f(36, 40);
                    glVertex2f(30, 10);
                    glVertex2f(36, 10);
                glEnd();
                glBegin(GL_TRIANGLES);
                    glVertex2f(44, 40);
                    glVertex2f(44, 10);
                    glVertex2f(50, 40);
                    glVertex2f(50, 40);
                    glVertex2f(44, 10);
                    glVertex2f(50, 10);
                glEnd();
            } else {
                glBegin(GL_TRIANGLES);
                    glVertex2f(32, 40);
                    glVertex2f(52, 25);
                    glVertex2f(32, 10);
                glEnd();
            }
        };
    }
}