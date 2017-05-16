#pragma once

#include "GLEnvironment.hpp"
#include <atomic>

namespace graphics {
    class Graphics
    {
      public:
        Graphics(int const windowWidth, 
                 int const windowHeight,
                 GLEnvironment & glEnviro);
        void display();
        void reshape(int const w, int const h);
        void passiveMouseFunc(int const x, int const y);
        void keyboardHandler(int const key, 
                             int const x, 
                             int const y);
      private:
        int m_windowWidth;
        int m_windowHeight;
        GLEnvironment & m_glEnviro;
        std::atomic<double> m_viewDistance;
    };
}