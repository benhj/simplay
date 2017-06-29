// Copyright (c) 2017 Ben Jones

#pragma once

#include <OpenGL/gl.h>

namespace graphics { namespace detail {
    inline void setScene(int const windowWidth,
                         int const windowHeight,
                         double const viewDistance,
                         double const orientation,
                         double const centerX = 0,
                         double const centerY = 0)
    {
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho((-windowWidth/2)*viewDistance + centerX, 
                (windowWidth/2)*viewDistance + centerX, 
                (-windowHeight/2)*viewDistance + centerY, 
                (windowHeight/2)*viewDistance + centerY, 
                -10000, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(orientation, 0.0, 0.0, 1.0);
    }
}
}