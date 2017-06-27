// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "GLAnimat.hpp"
#include "GLCompass.hpp"
#include "GLAxis.hpp"
#include "SetScene.hpp"
#include "WorldToScreen.hpp"
#include "ThreadRunner.hpp"

#include "model/AnimatWorld.hpp"

#include <OpenGL/gl.h>
#include <vector>
#include <atomic>
#include <chrono>

namespace graphics {
    class GLEnvironment
    {
      public:
        GLEnvironment(int & windowWidth,
                      int & windowHeight,
                      model::AnimatWorld & animatWorld)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_viewDistance(0.4)
        , m_worldOrientation(0)
        , m_animatWorld(animatWorld)
        {
            auto const popSize = m_animatWorld.getPopSize();
            m_glAnimats.reserve(popSize);
            for (int p = 0; p < popSize; ++p) {
                m_glAnimats.emplace_back(m_animatWorld.animat(p));
            }
        }

        void draw()
        {
            if (m_displayAxis) { 
                graphics::GLAxis(m_windowWidth, 
                                 m_windowHeight,
                                 m_viewDistance,
                                 m_worldOrientation).draw();
            }
            detail::setScene(m_windowWidth, 
                             m_windowHeight, 
                             m_viewDistance, 
                             m_worldOrientation);
            for (int p = 0; p < m_animatWorld.getPopSize(); ++p) {
                m_glAnimats[p].draw();
            }
            if (m_displayCompass) {
                graphics::GLCompass(m_windowWidth, 
                                    m_windowHeight,
                                    m_viewDistance,
                                    m_worldOrientation).draw();
            }
        }

        void compassOn()
        {
            m_displayCompass = true;
        }

        void compassOff()
        {
            m_displayCompass = false;
        }

        void toggleAxisDisplay()
        {
            m_displayAxis = !m_displayAxis;
        }

        void checkForAnimatHighlight(double x, double y)
        {
            /// Hacky as fuck. Nevertheless, does the trick!
            for (int p = 0; p < m_animatWorld.getPopSize(); ++p) {
                auto & animat = m_animatWorld.animat(p);
                auto centralPoint = animat.getCentralPoint();
                auto & pos = centralPoint.first;
                auto cx = pos.m_vec[0];
                auto cy = pos.m_vec[1];
                double sx, sy;
                detail::worldToScreen(cx, -cy, sx, sy);
                auto diffX = (sx - x);
                auto diffY = (sy - y);
                auto diffXSq = std::sqrt(diffX * diffX) * m_viewDistance;
                auto diffYSq = std::sqrt(diffY * diffY) * m_viewDistance;
                if (diffXSq < 5 && diffYSq < 5) {
                    m_glAnimats[p].highlight();
                } else {
                    m_glAnimats[p].dehighlight();
                }
            }
        }

        void spinLeft()
        {
            auto computed = m_worldOrientation.load();
            computed += 5;
            m_worldOrientation = computed;
        }

        void spinRight()
        {
            auto computed = m_worldOrientation.load();
            computed -= 5;
            m_worldOrientation = computed;
        }

        void selectAnimat()
        {
            for (auto & glAnimat : m_glAnimats) {
                if(glAnimat.isHighlighted()) {
                    glAnimat.select();
                } else {
                    glAnimat.deselect();
                }
            }
        }

        void setViewDistance(double const newDistance)
        {
            m_viewDistance = newDistance;
        }

        void updateWidthHeight(int const width, int const height)
        {
            m_windowWidth = width;
            m_windowHeight = height;
            detail::setScene(width, height, m_viewDistance, m_worldOrientation);
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        std::atomic<double> m_viewDistance;
        std::atomic<double> m_worldOrientation;
        model::AnimatWorld & m_animatWorld;
        std::vector<graphics::GLAnimat> m_glAnimats;
        Color m_backGoundColour { 209, 220, 235};
        std::atomic<bool> m_displayAxis{true};
        std::atomic<bool> m_displayCompass{false};
        detail::ThreadRunner m_threadRunner;
    };
}