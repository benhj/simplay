/// Copyright (c) 2017 Ben Jones

#pragma once

namespace simulator {
    /// Defines a control mechanism for the animat
    /// Will probably implement a neural version of this
    class Controller
    {
      public:
        virtual double getLeftMotorOutput(int const i) const = 0;
        virtual double getRightMotorOutput(int const i) const = 0;
        virtual void update() = 0;
    };
}