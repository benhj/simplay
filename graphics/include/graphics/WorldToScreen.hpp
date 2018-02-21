// Copyright (c) 2017 Ben Jones

#pragma once

#include "RetinaScalar.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

namespace graphics { namespace detail {

    inline void worldToScreen(double const wx, double const wy,
                              double & sx, double & sy)
    {
        double sz;
        GLdouble projection[16];
        GLdouble modelview[16];
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        gluProject(wx, wy, 0, modelview, projection, viewport, &sx, &sy, &sz);
    }

}
}