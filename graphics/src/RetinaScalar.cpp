#include "graphics/RetinaScalar.hpp"
#include <mutex>

namespace {
    std::once_flag flag;
}

namespace graphics {  namespace detail {
    float retinaScalar(GLFWwindow * window)
    {
        static float display_scale = 1.0f;
        if(window) {
            std::call_once(flag, [&](){ 
                int fb_width = 0;
                int sc_width = 0;
                glfwGetFramebufferSize( window, &fb_width, NULL );
                glfwGetWindowSize( window, &sc_width, NULL );
                display_scale = fb_width / (float)sc_width;
            });
        }
        return display_scale;
    }

    void lineWidth(GLfloat orig)
    {
        glLineWidth(orig * retinaScalar());
    }
}
}