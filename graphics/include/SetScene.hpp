#pragma once

#include <OpenGL/gl.h>

namespace graphics { namespace detail {
    inline void setScene(int const windowWidth,
                         int const windowHeight,
                         double const viewDistance,
                         double const orientation)
    {
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho((-windowWidth/2)*viewDistance, 
                (windowWidth/2)*viewDistance, 
                (-windowHeight/2)*viewDistance, 
                (windowHeight/2)*viewDistance, 
                -10000, 10000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(orientation, 0.0, 0.0, 1.0);
    }
}
}