// GLConsole, based on benhj/glconsole (github). 
//
// This 'ere software is hacky as fuck. Use at your peril.
// I'd appreciate a mention if you do. But if you don't,
// I'm not going to lose sleep over it.
// Ben H.D. Jones in the year of 2018

#pragma once

#include "RetinaScalar.hpp"
#include "glfreetype/TextRenderer.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <cctype>
#include <functional>
#include <sstream>

namespace glconsole {
    class GLConsole
    {
      public:
        GLConsole(int & windowWidth,
                  int & windowHeight,
                  int const width, 
                  int const height,
                  int const x,
                  int const y,
                  int const fontSize,
                  std::string const & fontPath,
                  std::atomic<float> & opacity)
          : m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
          , m_width(width)
          , m_height(height)
          , m_widthRatio((double)m_width / (double)m_windowWidth)
          , m_heightRatio((double)m_height / (double)m_windowHeight)
          , m_x(x)
          , m_y(y - m_windowHeight/2)
          , m_buffer()
          , m_linesCommitted()
          , m_prompt(">> |")
          , m_commandBegin(4)
          , m_commandEnd(4)
          , m_opacity(opacity)
        {
            m_font.init(fontPath.c_str(), fontSize * graphics::detail::retinaScalar());

            // Begin what the user will see!
            m_buffer << m_prompt;
        }

        void updateWidthAndHeightAndPromptPosition()
        {
            m_width = (int)(m_windowWidth * m_widthRatio);
            //m_height = (int)(m_windowHeight * m_heightRatio);
            m_y = (m_windowHeight - m_height) - m_windowHeight/2;
        }

        void display()
        {  
            updateWidthAndHeightAndPromptPosition();
            // draw filled rectangle for background
            glColor4f(0, 0, 0, m_opacity);

            glBegin(GL_QUADS);
                glVertex2f(m_x - m_width, m_y);
                glVertex2f(m_x + m_width, m_y);
                glVertex2f(m_x + m_width, m_y + m_height);
                glVertex2f(m_x - m_width, m_y + m_height);
            glEnd();

            // Green text
            glPushMatrix();
            glLoadIdentity();
            glColor4f(0, 0xff, 0, m_opacity);
            std::stringstream ss;
            // Offset from bottom of screen
            auto const ypos = (m_windowHeight * graphics::detail::retinaScalar() - (m_height / 2));
            glfreetype::print(m_font, m_x, ypos, m_buffer.str());
            glPopMatrix();
        }

        void setCallback(std::function<void(std::string const &)> const & callback)
        {
            m_callback = callback;
        }

        void keyHandler(int const key, 
                        int const scancode, 
                        int const action, 
                        int const mods) 
        {
            if (action == GLFW_PRESS) {
                if (key != GLFW_KEY_BACKSPACE) {

                    // Figure out if char should be upper or lowercase.
                    // By default, GLFW seems to yield uppercase chars.
                    auto upperCase = false;
                    if(mods == GLFW_MOD_SHIFT) {
                        upperCase = true;
                    }

                    // Don't print out shift key chars
                    if(key != GLFW_KEY_LEFT_SHIFT &&
                       key != GLFW_KEY_RIGHT_SHIFT) {

                        // Before appending the new char, need
                        // to delete the '|' char.
                        auto before = m_buffer.str();
                        auto after = before.substr(0, before.size() - 1);
                        m_buffer.str("");
                        m_buffer << after;

                        if(key != GLFW_KEY_ENTER) {
                            // Now append the char
                            auto casedChar = char(key);
                            if(!upperCase) {
                                casedChar = std::tolower(casedChar);
                            }

                            // ...finishing with the '|' char.
                            m_buffer << casedChar << '|';

                            // Store where we are in the buffer
                            // for figuring out the entered command.
                            ++m_commandEnd;
                        } else {

                            auto beforeNewLine = m_buffer.str();

                            // New line
                            m_buffer.str("");
                            m_buffer << m_prompt;

                            // Store buffer so far (in aid of
                            // a bug-free backspace process).
                            m_linesCommitted = m_buffer.str();

                            // Parse out the entered text
                            std::string parsedCommand(std::begin(beforeNewLine) +
                                                      m_commandBegin - 1,
                                                      std::begin(beforeNewLine) +
                                                      m_commandEnd);
                            
                            // Trigger the callback if one has been set.
                            if(m_callback) {
                                m_callback(parsedCommand);
                            }

                            // Reset integer iterators
                            m_commandBegin = m_linesCommitted.length();
                            m_commandEnd = m_commandBegin;
                        }
                    }
                } else {
                    handleBackspace();
                }
            }
        }

        bool mouseIsOver(int const x, int y) const
        {
            // Hacky offset. God this code is awful.
            y -= m_height;
            if ((x >= m_x - m_width && 
                 x <= m_x + m_width &&
                 y <= m_y + m_height - (m_windowHeight / 2) &&
                 y >= m_y - (m_windowHeight / 2))) {
                return true;
            }
            return false;
        }

      private:

        /// Window width and height
        int & m_windowWidth;
        int & m_windowHeight;

        /// Width of text area
        int m_width;

        /// Height of text area
        int m_height;

        /// Width and height window ratios
        double m_widthRatio;
        double m_heightRatio;

        /// X and Y position of where text is rendered
        /// relative to width and height
        int m_x;
        int m_y;

        /// Stores information related to the font
        glfreetype::font_data m_font;

        /// Stores text typed so far
        std::stringstream m_buffer;

        /// Stores previously entered line data.
        std::string m_linesCommitted;

        /// The actual prompt, hardcoded to ">>"
        std::string m_prompt;

        /// Store command position
        int m_commandBegin;
        int m_commandEnd;

        /// The callback that will be triggered on enter.
        std::function<void(std::string const &)> m_callback;

        /// Window opacity
        std::atomic<float> & m_opacity;

        /// Does what it sayd.
        void handleBackspace()
        {
            auto before = m_buffer.str();

            // Only permit stuff after ">> ".
            if(!before.empty() && before.length() > 4) {
                auto after = before.substr(0, before.length() - 2);

                // Make sure that we don't delete newline chars
                // and ">> " in the case of a newline.
                if(m_linesCommitted.empty() ||
                   after.length() >= m_linesCommitted.length() - 1) {
                    m_buffer.str("");
                    m_buffer << after << "|";
                    --m_commandEnd;
                }
            }
        }
    };
}