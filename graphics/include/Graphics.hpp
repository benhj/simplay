#pragma once

#include "GLEnvironment.hpp"
#include <atomic>

namespace graphics {
    class Graphics
    {
      public:
        explicit Graphics(GLEnvironment & glEnviro);
        Graphics() = delete;
        void display();
        void reshape(int const w, int const h);
        void passiveMouseFunc(int const x, int const y);
        void keyboardHandler(int const key, 
                             int const x, 
                             int const y);
      private:
        GLEnvironment & m_glEnviro;
        std::atomic<double> m_viewDistance;
    };
}