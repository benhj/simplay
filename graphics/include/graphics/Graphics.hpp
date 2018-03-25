#pragma once

#include "GLEnvironment.hpp"
#include "GLGUIElement.hpp"
#include "ThreadRunner.hpp"
#include "GLConsole.hpp"
#include <atomic>
#include <memory>
#include <vector>

namespace graphics {
    class Graphics
    {
      public:
        explicit Graphics(int & windowWidth, 
                          int & windowHeight, 
                          GLEnvironment & glEnviro,
                          detail::ThreadRunner & threadRunner);
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

        void addGUIElement(std::shared_ptr<GLGUIElement> element);

      private:

        /// Dimensions of window
        int & m_windowWidth;
        int & m_windowHeight;

        /// Represents the drawing of the animat environment
        GLEnvironment & m_glEnviro;

        /// To run stuff async
        detail::ThreadRunner & m_threadRunner;

        /// Zoom parameter
        std::atomic<double> m_viewDistance;

        /// Testing out some GUI ideas
        std::vector<std::shared_ptr<GLGUIElement>> m_guiElements;

        /// Opacity of command window
        std::atomic<float> m_consoleOpacity;

        /// A command window that will slide out from the top
        glconsole::GLConsole m_console;

        /// Display command console?
        std::atomic<bool> m_displayConsole;

        /// Console has focus when mouse is over it
        std::atomic<bool> m_consoleHasFocus;

        void handleKeyDown(int const key);
        void handleKeyUp(int const key);
        void handleKeyContinuous(int const key);

        /// Draw GUI elements (makes sure we're in correct view/matrix mode)
        void drawGUIElements();
        void drawGUIElementsSetup();
        void drawGUIElementsTearDown();

        /// Fade in the console window
        void fadeInConsole();


    };
}