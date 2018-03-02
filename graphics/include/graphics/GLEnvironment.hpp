/// Copyright (c) 2017 - present Ben Jones

#pragma once

#include "Color.hpp"
#include "GLAnimat.hpp"
#include "GLCompass.hpp"
#include "GLAxis.hpp"
#include "GLButton.hpp"
#include "RetinaScalar.hpp"
#include "SetScene.hpp"
#include "WorldToScreen.hpp"
#include "ThreadRunner.hpp"

#include "model/AnimatWorld.hpp"
#include "glfreetype/TextRenderer.hpp"

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
                      model::AnimatWorld & animatWorld,
                      detail::ThreadRunner & threadRunner)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_animatWorld(animatWorld)
        , m_threadRunner(threadRunner)
        , m_viewDistance(0.4)
        , m_worldOrientation(0)
        , m_centerX(0)
        , m_centerY(0)
        , m_oldFlyXDiv(0)
        , m_oldFlyYDiv(0)
        , m_selected(-1)
        // , m_generationText()
        {
            auto const popSize = m_animatWorld.getPopSize();
            m_glAnimats.reserve(popSize);
            for (int p = 0; p < popSize; ++p) {
                m_glAnimats.emplace_back(m_animatWorld.animat(p));
            }
            // m_generationText.init("/Library/Fonts/Arial.ttf", 
            //                       25 * detail::retinaScalar());
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
            // glPushMatrix();
            // glLoadIdentity();
            // // Blue texts
            // glColor3ub(0,0,0xff);
            // auto realWidth = m_windowWidth * detail::retinaScalar();
            // glfreetype::print(m_generationText, realWidth / 1.5, 
            //                   10 * detail::retinaScalar(), "Generation: ");
            // glPopMatrix();
        }

        std::atomic<double> & getWorldOrientation()
        {
            return m_worldOrientation;
        }

        std::atomic<double> & getViewDistance()
        {
            return m_viewDistance;
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
                glAnimat.checkForHighlight(x, m_windowHeight - y, 
                                           m_viewDistance, 
                                           m_centerX, m_centerY);
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

        // WARNING -- the following function is hacky as fuck!!
        void flyIn()
        {
            // Whether to 'fly in' or 'fly out'
            static bool in = true;

            // Figure out center point to screen point
            if (in) {
                if(m_selected > -1) {
                    auto & flAnimat = m_glAnimats[m_selected];
                    auto & animat = flAnimat.animatRef();
                    auto centralPoint = animat.getCentralPoint();
                    auto & pos = centralPoint.first;
                    auto cx = pos.m_vec[0];
                    auto cy = pos.m_vec[1];
                    double sx, sy;

                    detail::worldToScreen(cx, -cy, sx, sy);
                    auto width = m_windowHeight * detail::retinaScalar();
                    auto height = m_windowHeight * detail::retinaScalar();
                    auto fx = (sx * m_viewDistance) - ((width / 2) * m_viewDistance) + m_centerX;
                    auto fy = (sy * m_viewDistance) - ((height / 2) * m_viewDistance) + m_centerY;
                    auto distx = std::sqrt((fx - m_centerX) * (fx - m_centerX));
                    auto disty = std::sqrt((fy - m_centerY) * (fy - m_centerY));

                    distx /= detail::retinaScalar();
                    disty /= detail::retinaScalar();

                    auto centerDivX = distx / 10.0;
                    auto centerDivY = disty / 10.0;
                    processFlyIn(centerDivX, centerDivY, fx, fy);
                    in = false;
                }
            } else {
                processFlyOut();
                in = true;
            }
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        model::AnimatWorld & m_animatWorld;
        detail::ThreadRunner & m_threadRunner;

        /// Index of the currently selected animat
        std::atomic<int> m_selected;
        std::atomic<double> m_viewDistance;
        std::atomic<double> m_worldOrientation;
        std::atomic<double> m_centerX;
        std::atomic<double> m_centerY;
        double m_oldFlyXDiv;
        double m_oldFlyYDiv;
        double m_oldZoomIt;
        std::vector<graphics::GLAnimat> m_glAnimats;
        Color m_backGoundColour { 209, 220, 235};
        std::atomic<bool> m_displayAxis{true};
        std::atomic<bool> m_displayCompass{false};

        // Display current generation
        // glfreetype::font_data m_generationText;


        void processFlyIn(double const centerDivX, 
                          double const centerDivY,
                          double const fx,
                          double const fy)
        {
            m_oldFlyXDiv = 0;
            m_oldFlyYDiv = 0;
            m_viewDistance = 0.4;
            m_oldZoomIt = (m_viewDistance - 0.15) / 10.0;
            std::function<void()> func([=]() {
                auto zoomVal = m_viewDistance.load();
                for(int i = 0; i < 10; ++i) {
                    auto valX = m_centerX.load();

                    if (valX < fx - centerDivX) {
                        valX += centerDivX;
                        m_oldFlyXDiv += centerDivX;
                    } else if (valX > fx + centerDivX) {
                        valX -= centerDivX;
                        m_oldFlyXDiv -= centerDivX;
                    }
                    m_centerX.store(valX);

                    auto valY = m_centerY.load();
                    if (valY > fy - centerDivY) {
                        valY += centerDivY;
                        m_oldFlyYDiv += centerDivY;
                    } else if (valY < fy + centerDivY) {
                        valY -= centerDivY;
                        m_oldFlyYDiv -= centerDivY;
                    }
                    m_centerY.store(valY);
                    zoomVal -= m_oldZoomIt;
                    m_viewDistance.store(zoomVal);

                    usleep(10000);
                }
            });

            m_threadRunner.add(func);
        }

        void processFlyOut()
        {
            m_oldFlyYDiv /= 10;
            m_oldFlyXDiv /= 10;
            std::function<void()> func([=]() {
                auto zoomVal = m_viewDistance.load();
                for(int i = 0; i < 10; ++i) {
                    auto valX = m_centerX.load();
                    valX -= m_oldFlyXDiv;
                    m_centerX.store(valX);

                    auto valY = m_centerY.load();
                    valY -= m_oldFlyYDiv;
                    m_centerY.store(valY);
                    zoomVal += m_oldZoomIt;
                    m_viewDistance.store(zoomVal);
                    usleep(10000);
                }
            });

            m_threadRunner.add(func);
        }
    };
}