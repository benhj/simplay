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
        GLCircularDial(GLFWwindow * window,
                       int xLocation,
                       int yLocation,
                       double radius,
                       int const startAngle,
                       detail::ThreadRunner & threadRunner);
        GLCircularDial() = delete;

        void draw() override;

        // /// fade in the button when hovering over
        // void fadeIn()
        // {
        //     auto const inc = (0.9 / 30);
        //     for(int i = 0; i < 30; ++i) {
        //         auto val = m_opacity.load();
        //         val += inc;
        //         m_opacity.store(val);
        //         usleep(10000);
        //     }
        // }

        // /// fade out the button on pointer exit
        // void fadeOut()
        // {
        //     auto const inc = (0.9 / 30);
        //     for(int i = 0; i < 30; ++i) {
        //         auto val = m_opacity.load();
        //         val -= inc;
        //         m_opacity.store(val);
        //         usleep(10000);
        //     }
        // }

        /// When pointer over button, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        void mouseIsOver(int const x, int const y) override;

        void handleClick(int const action) override;

        //void installHandler(std::function<void(bool const)> const & handler);

        void setColor(Color const & color);

      private:
        GLFWwindow* m_window;
        int m_xLocation;
        int m_yLocation;
        int m_derivedX;
        int m_derivedY;
        //int m_windowHeight;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Button width and height
        double m_radius;

        /// Angle of dial
        int m_angle;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of button
        std::atomic<double> m_opacity;

        /// Callback to trigger on click
        //std::function<void(bool const)> m_handler;

        // Default colour
        Color m_buttonColor { 155, 155, 155 };
    };


}