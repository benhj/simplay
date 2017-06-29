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
#include <unistd.h>

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
        , m_centerX(0)
        , m_centerY(0)
        , m_animatWorld(animatWorld)
        , m_selected(-1)
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
                             m_worldOrientation,
                             m_centerX,
                             m_centerY);
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

        void checkForAnimatHighlight(double const x, double const y)
        {
            for (auto & glAnimat : m_glAnimats) {
                glAnimat.checkForHighlight(x, y, m_viewDistance, m_centerX, -m_centerY);
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
            int i = 0;
            for (auto & glAnimat : m_glAnimats) {
                if (glAnimat.handleSelection()) {
                    m_selected = i;
                }
                ++i;
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
            detail::setScene(width, 
                             height, 
                             m_viewDistance, 
                             m_worldOrientation,
                             m_centerX,
                             m_centerY);
        }

        void flyIn()
        {
            // Figure out center point to screen point
            if(m_selected > -1) {
                auto & flAnimat = m_glAnimats[m_selected];
                auto & animat = flAnimat.animatRef();
                auto centralPoint = animat.getCentralPoint();
                auto & pos = centralPoint.first;
                auto cx = pos.m_vec[0];
                auto cy = pos.m_vec[1];
                double sx, sy;
                detail::worldToScreen(cx, -cy, sx, sy);
                auto fx = (sx * m_viewDistance) - (m_windowWidth / 2) * m_viewDistance + m_centerX;
                auto fy = (sy * m_viewDistance) - (m_windowHeight / 2) * m_viewDistance + m_centerY;

                auto distx = std::sqrt((fx - m_centerX) * (fx - m_centerX));
                auto disty = std::sqrt((fy - m_centerY) * (fy - m_centerY));

                std::function<void()> func([=]() {
                    auto centerDivX = distx / 10.0;
                    auto centerDivY = disty / 10.0;

                    for(int i = 0; i < 10; ++i) {
                        auto valX = m_centerX.load();
                        if (valX < fx - centerDivX) {
                            valX += centerDivX;
                        } else if (valX > fx + centerDivX) {
                            valX -= centerDivX;
                        }
                        m_centerX.store(valX);

                        auto valY = m_centerY.load();
                        if (valY > fy - centerDivY) {
                            valY += centerDivY;
                        } else if (valY < fy + centerDivY) {
                            valY -= centerDivY;
                        }
                        m_centerY.store(valY);

                        usleep(10000);
                    }
                });

                m_threadRunner.add(func);
            }
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;

        /// Index of the currently selected animat
        std::atomic<int> m_selected;
        std::atomic<double> m_viewDistance;
        std::atomic<double> m_worldOrientation;
        std::atomic<double> m_centerX;
        std::atomic<double> m_centerY;
        model::AnimatWorld & m_animatWorld;
        std::vector<graphics::GLAnimat> m_glAnimats;
        Color m_backGoundColour { 209, 220, 235};
        std::atomic<bool> m_displayAxis{true};
        std::atomic<bool> m_displayCompass{false};
        detail::ThreadRunner m_threadRunner;
    };
}