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
        void passiveMouseFunc(double const x, double const y);
        void keyboardHandler(int const key, 
                             int const scancode, 
                             int const action, 
                             int const mods); 
      private:
        GLEnvironment & m_glEnviro;
        std::atomic<double> m_viewDistance;
        void handleKeyDown(int const key);
        void handleKeyUp(int const key);
        void handleKeyContinuous(int const key);
    };
}