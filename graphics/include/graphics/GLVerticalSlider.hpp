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

    class GLVerticalSlider : public GLGUIElement
    {
      public:
        GLVerticalSlider(GLFWwindow * window,
                         int windowOffset,
                         int width,
                         detail::ThreadRunner & threadRunner);
        GLVerticalSlider() = delete;

        void draw() override;

        /// When pointer over element, a 'fade-in'
        /// is triggered, or a 'fade-out' on exit.
        void mouseIsOver(int const x, int const y) override;

        void handleClick(int const action) override;

        void installHandler(std::function<void(double const value)> const & handler);

        /// Updates the slider position based on the passed-in value
        void updateSliderPosition(double const value);

      private:
        GLFWwindow* m_window;
        int m_derivedX;
        int m_derivedY;
        std::atomic<int> m_windowWidth;
        std::atomic<int> m_windowHeight;
        int m_windowOffset;
        int m_width;
        std::atomic<int> m_length;

        /// Position 0 is center of slider.
        /// Up in -ve, down is +ve
        int m_sliderPosition;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        /// Mouse hovering over button, true
        std::atomic<bool> m_entered;

        /// True when mouse over, false otherwise
        std::atomic<bool> m_state;

        /// For fade-level of element
        std::atomic<double> m_opacity;

        /// Default colour
        Color m_buttonColor { 155, 155, 155 };

        /// 'Value' of the slider (up is +ve)
        double m_currentVal;

        /// Callback to trigger some change based on slider position
        std::function<void(double const value)> m_handler;

        /// Draw slider knob at some position on slider
        void drawSliderKnob();

        /// fade in the element when hovering over
        void fadeIn();

        /// fade out the element on pointer exit
        void fadeOut();
    };


}