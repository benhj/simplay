/// Copyright (c) 2017 Ben Jones

#pragma once

#include <OpenGL/gl.h>

namespace graphics {
    struct Color {
        double R;
        double G;
        double B;
    };
    namespace detail {
        inline void setColor(Color const & color, double const opacity = 1.0)
        {
            glColor4f(color.R / 255.0, color.G / 255.0, color.B / 255.0, opacity);
        }
    }
}