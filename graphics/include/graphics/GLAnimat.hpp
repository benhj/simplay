// Copyright (c) 2017-present Ben Jones

#pragma once

#include "Color.hpp"
#include "ThreadRunner.hpp"
#include "model/Animat.hpp"
#include <atomic>
#include <memory>

namespace graphics {

    struct GLAnimat
    {
        GLAnimat(std::shared_ptr<model::Animat> animat,
                 detail::ThreadRunner & threadRunner);

        GLAnimat() = delete;

        void draw();

        bool handleSelection();

        void checkForHighlight(double x, 
                               double y,
                               double const viewDistance,
                               double const centerX,
                               double const centerY);

        std::shared_ptr<model::Animat> animatRef();

        void updateAnimat(std::shared_ptr<model::Animat> animat);

      private:

        std::shared_ptr<model::Animat> m_animat;

        /// Threads the fade-in or fade-out process
        detail::ThreadRunner & m_threadRunner;

        Color m_basicColor { 87, 89, 92 };
        Color m_segmentColor { 197, 217, 200 };
        Color m_antennaeColor { 222, 153, 153 };
        Color m_bigCircleColor { 222, 90, 90 };

        /// If mouse pointer over animat, draw big bounding circle around it
        /// to indicate it's 'selected'
        std::shared_ptr<std::atomic<bool>> m_highlighted;

        /// To indicate animat is selected in which thicker
        /// bounding circle will be drawn around animat
        std::shared_ptr<std::atomic<bool>> m_selected;

        /// For fade-level of button
        std::shared_ptr<std::atomic<double>> m_opacity;

        std::shared_ptr<std::mutex> m_mutex;

        void drawBody();
        void drawAntennae();
        void drawBoundingCircles();
        void drawBigBoundingCircle();
        void showPopNumber(std::pair<physics::Vector3, double> const & point);

        /// fade in text
        void fadeIn();
        void fadeOut();
    };
}