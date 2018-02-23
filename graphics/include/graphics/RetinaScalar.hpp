#pragma once
#include <GLFW/glfw3.h>
namespace graphics { namespace detail {
    float retinaScalar(GLFWwindow * window = nullptr);
    void lineWidth(GLfloat orig);
}
}