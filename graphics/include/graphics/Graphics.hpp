#pragma once

#include "GLEnvironment.hpp"
#include "GLButton.hpp"
#include <atomic>

namespace graphics {
    class Graphics
    {
      public:
        explicit Graphics(int & windowWidth, 
                          int & windowHeight, 
                          GLEnvironment & glEnviro);
        Graphics() = delete;
        void display();
        void reshape(int const w, int const h);
        void passiveMouseFunc(double const x, double const y);
        void keyboardHandler(int const key, 
                             int const scancode, 
                             int const action, 
                             int const mods); 
        void clickHandler(int const button, 
                          int const action, 
                          int const mods);
      private:

        /// Dimensions of window
        int & m_windowWidth;
        int & m_windowHeight;

        /// Represents the drawing of the animat environment
        GLEnvironment & m_glEnviro;

        /// Zoom parameter
        std::atomic<double> m_viewDistance;

        /// Testing out some GUI ideas
        GLButton m_testButton;
        void handleKeyDown(int const key);
        void handleKeyUp(int const key);
        void handleKeyContinuous(int const key);

        /// Draw GUI elements (makes sure we're in correct view/matrix mode)
        void drawGUIElements();
        void drawGUIElementsSetup();
        void drawGUIElementsTearDown();


    };
}