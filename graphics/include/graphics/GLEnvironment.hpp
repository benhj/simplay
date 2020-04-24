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
#include <functional>
#include <sstream>
#include <unistd.h>

namespace graphics {

    struct FlyInfo {
        double centerDivX;
        double centerDivY;
        double fx;
        double fy;
    };

    class GLEnvironment
    {
      public:
        GLEnvironment(int & windowWidth,
                      int & windowHeight,
                      double & viewDistance,
                      model::AnimatWorld & animatWorld,
                      detail::ThreadRunner & threadRunner)
        : m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_animatWorld(animatWorld)
        , m_threadRunner(threadRunner)
        , m_viewDistance(viewDistance)
        , m_worldOrientation(0)
        , m_centerX(0)
        , m_centerY(0)
        , m_oldFlyXDiv(0)
        , m_oldFlyYDiv(0)
        , m_selected(-1)
        , m_selectedOrig(-1)
        , m_generationText()
        , m_trackCounter(0)
        {
            auto const popSize = m_animatWorld.getPopSize();
            m_glAnimats.reserve(popSize);
            for (int p = 0; p < popSize; ++p) {
                m_glAnimats.emplace_back(m_animatWorld.animat(p),
                                         m_threadRunner);
            }
            m_generationText.init("../fonts/digital-7.ttf", 
                                   25 * detail::retinaScalar());
        }

        void updateAnimat(int const index,
                          std::shared_ptr<model::Animat> animat)
        {
            std::lock_guard<std::mutex> lg(m_mutex);
            m_glAnimats[index].updateAnimat(std::move(animat));
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
            {
                std::lock_guard<std::mutex> lg(m_mutex);
                for (int p = 0; p < m_animatWorld.getPopSize(); ++p) {
                    m_glAnimats[p].draw();
                }
            }

            graphics::GLCompass(m_windowWidth, 
                                m_windowHeight,
                                m_viewDistance,
                                m_worldOrientation).draw();
            
            glPushMatrix();
            glLoadIdentity();
            // Blue texts
            glColor4f(0.5, 0.5, 0.9, 0.9);
            auto realWidth = m_windowWidth * detail::retinaScalar();
            std::stringstream ss;
            ss << "GEN " << m_animatWorld.getOptimizationCount();
            glfreetype::print(m_generationText, 200, 
                              35 * detail::retinaScalar(), ss.str());
            glPopMatrix();
        }

        std::atomic<double> & getWorldOrientation()
        {
            return m_worldOrientation;
        }

        double & getViewDistance()
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
            std::lock_guard<std::mutex> lg(m_mutex);
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

        void track()
        {
            std::lock_guard<std::mutex> lg(m_mutex);
            if(m_selected > -1) {
                m_glAnimats[m_selected].track();
                if(m_selected != m_selectedOrig && m_selectedOrig > -1) {
                    m_glAnimats[m_selectedOrig].untrack();
                    m_centerY = 0;
                    m_centerX = 0;
                    m_viewDistance = 0.4;
                } else if(m_glAnimats[m_selected].hadToWrap()) {
                    m_centerY = 0;
                    m_centerX = 0;
                    m_viewDistance = 0.4;
                }

                auto fi = doFlyIn();
                if(m_trackCounter == 0) {
                    processFlyIn(fi.centerDivX, fi.centerDivY, fi.fx, fi.fy, false);
                } else if(m_trackCounter > 20) {
                    moveWorldForTracking(fi.centerDivX, fi.centerDivY, fi.fx, fi.fy);
                }

                m_selectedOrig.store(m_selected);
                ++m_trackCounter;
            }
        }

        void untrack()
        {
            if(m_selected > -1) {
                if(m_glAnimats[m_selected].isTracked()) {
                    m_glAnimats[m_selected].untrack();
                    m_centerY = 0;
                    m_centerX = 0;
                    m_viewDistance = 0.4;
                    m_trackCounter = 0;
                }
            }
        }

        // WARNING -- the following function is hacky as fuck!!
        void flyIn()
        {
            std::lock_guard<std::mutex> lg(m_mutex);
            // Whether to 'fly in' or 'fly out'
            static bool in = true;

            // Figure out center point to screen point
            if (in) {
                if(m_selected > -1) {
                    auto fi = doFlyIn();
                    processFlyIn(fi.centerDivX, fi.centerDivY, fi.fx, fi.fy);
                    in = false;
                }
            } else {
                processFlyOut();
                in = true;
            }
        }

        void setZoomTrigger(std::function<void()> zoomTrigger)
        {
            m_zoomTrigger = std::move(zoomTrigger);
        }

      private:
        int & m_windowWidth;
        int & m_windowHeight;
        model::AnimatWorld & m_animatWorld;
        detail::ThreadRunner & m_threadRunner;

        /// Index of the currently selected animat
        std::atomic<int> m_selected;
        std::atomic<int> m_selectedOrig;

        double & m_viewDistance;
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

        std::mutex m_mutex;

        // Display current generation
        glfreetype::font_data m_generationText;

        // Tiggered when view distance is updated so that components
        // relying on this value (e.g. the vertical slider)
        // can be updated
        std::function<void()> m_zoomTrigger;

        long long m_trackCounter;

        FlyInfo doFlyIn()
        {
            auto & flAnimat = m_glAnimats[m_selected];
            auto animat = flAnimat.animatRef();
            auto centralPoint = animat->getCentralPoint();
            auto & pos = centralPoint.first;
            auto cx = pos.m_vec[0];
            auto cy = pos.m_vec[1];
            double sx, sy;

            detail::worldToScreen(cx, cy, sx, sy);

            auto width = m_windowWidth * detail::retinaScalar();
            auto height = m_windowHeight * detail::retinaScalar();

            FlyInfo fi;

            fi.fx = sx - (width / 2);
            fi.fy = (height / 2) - sy;
            auto distx = std::sqrt(fi.fx * fi.fx) * m_viewDistance;
            auto disty = std::sqrt(fi.fy * fi.fy) * m_viewDistance;

            // Fix above-computed values according to how
            // central world point has changed
            if(m_centerX > 0) {
                distx -= m_centerX;
            } else if(m_centerX < 0) {
                distx += m_centerX;
            }
            if(m_centerY > 0) {
                disty -= m_centerY;
            } else if(m_centerY < 0)  {
                disty += m_centerY;
            }

            distx /= detail::retinaScalar();
            disty /= detail::retinaScalar();

            fi.centerDivX = distx / 10.0;
            fi.centerDivY = disty / 10.0;
            return fi;
        }

        void update(double const valX,
                    double const valY)
        {
            m_centerX.store(valX);
            m_centerY.store(valY);
        }

        void moveWorldForTracking(double centerDivX, 
                                  double centerDivY,
                                  double const fx,
                                  double const fy)
        {
            centerDivX *= 10;
            centerDivY *= 10;
            auto valX = m_centerX.load();

            if (valX < fx - centerDivX) {
                valX += centerDivX;
                m_oldFlyXDiv += centerDivX;
            } else if (valX > fx + centerDivX) {
                valX -= centerDivX;
                m_oldFlyXDiv -= centerDivX;
            }

            auto valY = m_centerY.load();
            if (valY > fy - centerDivY) {
                valY += centerDivY;
                m_oldFlyYDiv += centerDivY;
            } else if (valY < fy + centerDivY) {
                valY -= centerDivY;
                m_oldFlyYDiv -= centerDivY;
            }

            update(valX, valY);
        }

        void processFlyIn(double const centerDivX, 
                          double const centerDivY,
                          double const fx,
                          double const fy,
                          bool const zoom = true)
        {
            m_oldFlyXDiv = 0;
            m_oldFlyYDiv = 0;
            if(zoom) {
                m_viewDistance = 0.4;
                m_oldZoomIt = (m_viewDistance - 0.15) / 10.0;
            } else {
                m_oldZoomIt = (0.4 - 0.15) / 10.0;
            }

            std::function<void()> func([=]() {
                auto zoomVal = m_viewDistance;
                for(int i = 0; i < 10; ++i) {
                    auto valX = m_centerX.load();

                    if (valX < fx - centerDivX) {
                        valX += centerDivX;
                        m_oldFlyXDiv += centerDivX;
                    } else if (valX > fx + centerDivX) {
                        valX -= centerDivX;
                        m_oldFlyXDiv -= centerDivX;
                    }

                    auto valY = m_centerY.load();
                    if (valY > fy - centerDivY) {
                        valY += centerDivY;
                        m_oldFlyYDiv += centerDivY;
                    } else if (valY < fy + centerDivY) {
                        valY -= centerDivY;
                        m_oldFlyYDiv -= centerDivY;
                    }

                    update(valX, valY);

                    if(zoom) {
                        zoomVal -= m_oldZoomIt;
                        m_viewDistance = zoomVal;

                        if(m_zoomTrigger) {
                            m_zoomTrigger();
                        }
                    }

                    usleep(10000);
                }
            });

            m_threadRunner.add(func);
        }

        void processFlyOut(bool zoom = true)
        {
            m_oldFlyYDiv /= 10;
            m_oldFlyXDiv /= 10;
            std::function<void()> func([=]() {
                auto zoomVal = m_viewDistance;
                for(int i = 0; i < 10; ++i) {
                    auto valX = m_centerX.load();
                    valX -= m_oldFlyXDiv;

                    auto valY = m_centerY.load();
                    valY -= m_oldFlyYDiv;

                    update(valX, valY);

                    if(zoom) {
                        zoomVal += m_oldZoomIt;
                        m_viewDistance = zoomVal;

                        if(m_zoomTrigger) {
                            m_zoomTrigger();
                        }
                    }

                    usleep(10000);
                }
            });

            m_threadRunner.add(func);
        }
    };
}