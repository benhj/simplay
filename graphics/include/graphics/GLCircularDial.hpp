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

    class GLCircularDial : public GLGUIElement
    {
      public:
        /// Note, in the following, startLevel is 0 to 100
        /// with 100 translating to 360 degrees
        GLCircularDial(GLFWwindow * window,
                       int xLocation,
                       int yLocation,
                       double radius,
                       int const startLevel,
                       detail::ThreadRunner & threadRunner);
        GLCircularDial() = delete;

        void draw() override;

        /// When pointer over button, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        void mouseIsOver(int const x, int const y) override;

        void handleClick(int const action) override;

        void installHandler(std::function<void(int const)> const & handler);

        void setColor(Color const & color);

        int getCurrentLevel() const;

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
        double m_radius;

        /// Angle of dial
        std::atomic<int> m_angle;

        /// Current percentage of dial
        std::atomic<int> m_level;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of button
        std::atomic<double> m_opacity;

        /// Callback to call on m_level change
        std::function<void(int const value)> m_handler;

        // Default colour
        Color m_buttonColor { 155, 155, 155 };

        /// fade in the button when hovering over
        void fadeIn();

        /// fade out the button on pointer exit
        void fadeOut();
    };


}