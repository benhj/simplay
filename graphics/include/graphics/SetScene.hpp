// Copyright (c) 2017-2018 Ben Jones

#pragma once

#include "RetinaScalar.hpp"
#include <OpenGL/gl.h>

namespace graphics { namespace detail {
    inline void setScene(int const windowWidth,
                         int const windowHeight,
                         double const viewDistance,
                         double const orientation,
                         double const centerX = 0,
                         double const centerY = 0)
    {
        auto const theWidth = windowWidth * retinaScalar();
        auto const theHeight = windowHeight * retinaScalar();
        glViewport(0, 0, theWidth, theHeight);
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