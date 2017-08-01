/// Copyright (c) 2017 Ben Jones

#pragma once

namespace graphics {

    class GLGUIElement
    {
      public:
        virtual void draw() = 0;
        virtual void mouseIsOver(int const x, int const y) = 0;
        virtual void handleClick(int const action) = 0;
    };

}