/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Animat.hpp"
#include "Controller.hpp"

namespace simulator {

    /// An agent will be composed of a model animat
    /// and a controller used to control that animat
    class Agent 
    {
      public:
        Agent(model::Animat & animat, Controller & controller);

        /// Actuate the animat based on control output
        /// Returns 0 on success, -1 if problem
        int update();

      private:
        model::Animat & m_animat;
        Controller & m_controller;
    };
}