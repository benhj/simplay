/// Copyright (c) 2017 Ben Jones

#pragma once

#include "Controller.hpp"
#include "model/Animat.hpp"
#include "neat/Network.hpp"
#include "physics/Vector3.hpp"

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

        /// Mutates the control mechanism
        void modifyController();

        /// Inherits genome from another agent
        void inheritNeat(Agent const & other);
        void inheritNeat(neat::Network const & net);

        /// Set the current central starting
        void recordStartPosition();

        void recordDistanceMoved();

        /// use start and end point to compute distance moved
        double distanceMoved() const;

        void resetController();

        neat::Network getNeatNet() const;

      private:

        /// The physical shell of the animat agent  
        model::Animat & m_animat;

        /// The neat network that will encode the 
        /// connectivity of the controller
        neat::Network m_neat;

        /// Controls the animat agent, it's movements etc.
        std::shared_ptr<Controller> m_controller;

        /// stores start position to compute distance travelled
        physics::Vector3 m_startPosition;

        double m_distanceMoved;

    };
}