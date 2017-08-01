/// Copyright (c) 2017 Ben Jones

#pragma once

#include "GLGUIElement.hpp"
#include "Color.hpp"
#include "ThreadRunner.hpp"
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <functional>

namespace graphics {

    class GLButton : public GLGUIElement
    {
      public:
        GLButton(GLFWwindow * window,
                 int xLocation,
                 int yLocation,
                 int width,
                 int height,
                 detail::ThreadRunner & threadRunner);
        GLButton() = delete;

        void draw() override;

        /// fade in the button when hovering over
        void fadeIn();

        /// fade out the button on pointer exit
        void fadeOut();

        /// When pointer over button, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        void mouseIsOver(int const x, int const y) override;

        void handleClick(int const action) override;

        void installHandler(std::function<void(bool const)> const & handler);

        void setOverlay(std::function<void(bool const)> const & overlay);

        void setColor(Color const & color);

      private:
        GLFWwindow* m_window;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;
        int m_windowHeight;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Button width and height
        int m_width;
        int m_height;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of button
        std::atomic<double> m_opacity;

        /// Callback to trigger on click
        std::function<void(bool const)> m_handler;

        /// For drawing a simple overlay
        std::function<void(bool const)> m_overLay;

        // Default colour
        Color m_buttonColor { 155, 155, 155 };
    };
}