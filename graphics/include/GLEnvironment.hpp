// Copyright (c) 2017 Ben Jones

#pragma once

#include "Color.hpp"
#include "AnimatWorld.hpp"
#include "GLAnimat.hpp"
#include "GLCompass.hpp"
#include "GLAxis.hpp"
#include "SetScene.hpp"
#include "WorldToScreen.hpp"

#include <OpenGL/gl.h>
#include <vector>
#include <atomic>

namespace graphics {
    class GLEnvironment
    {
      public:
        GLEnvironment(int & windowWidth,
                      int & windowHeight,
                      double & viewDistance,
                      double & worldOrientation,
                      simulator::AnimatWorld & animatWorld)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_viewDistance(viewDistance)
        , m_worldOrientation(worldOrientation)
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

        void checkForAnimatHighlight(int const x, int const y)
        {
            for (int p = 0; p < m_animatWorld.getPopSize(); ++p) {
                auto & animat = m_animatWorld.animat(p);
                auto centralPoint = animat.getCentralPoint();
                auto & pos = centralPoint.first;
                auto cx = pos.m_vec[0];
                auto cy = pos.m_vec[1];
                double sx, sy;
                detail::worldToScreen(cx, -cy, sx, sy);
                auto absDiffX = std::abs(sx - x);
                auto absDiffY = std::abs(sy - y);
                if (absDiffX < 40 && absDiffY < 40) {
                    m_glAnimats[p].highlight();
                } else {
                    m_glAnimats[p].dehighlight();
                }
            }
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        double & m_viewDistance;
        double & m_worldOrientation;
        simulator::AnimatWorld & m_animatWorld;
        std::vector<graphics::GLAnimat> m_glAnimats;
        Color m_backGoundColour { 209, 220, 235};
        std::atomic<bool> m_displayAxis{true};
        std::atomic<bool> m_displayCompass{false};
    };
}