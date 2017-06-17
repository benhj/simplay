/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Controller.hpp"
#include "model/Animat.hpp"
#include "neat/Network.hpp"

#include <memory>

namespace simulator {

    /// An agent will be composed of a model animat
    /// and a controller used to control that animat
    class Agent 
    {
      public:
        Agent(model::Animat & animat);

        /// Actuate the animat based on control output
        /// Returns 0 on success, -1 if problem
        int update();

      private:
        model::Animat & m_animat;
        neat::Network m_neat;
        std::shared_ptr<Controller> m_controller;

    };
}