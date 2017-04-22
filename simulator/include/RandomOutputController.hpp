/// Copyright (c) 2017 Ben Jones

#pragma once
#include "Controller.hpp"
#include <cstdlib>

namespace simulator {
    class RandomOutputController : public Controller
    {
      public:
        double getLeftMotorOutput(int const i) const override
        {
            return ((double) rand() / (RAND_MAX));
        }
        double getRightMotorOutput(int const i) const override
        {
            return ((double) rand() / (RAND_MAX));
        }
    };
}