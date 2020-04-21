/// Copyright (c) 2017-present Ben Jones

#pragma once

#include "Controller.hpp"
#include "model/Animat.hpp"
#include "model/SpeciesColour.hpp"
#include "neat/Network.hpp"
#include "physics/Vector3.hpp"

#include <memory>

namespace simulator {

    /// An agent will be composed of a model animat
    /// and a controller used to control that animat
    class Agent 
    {
      public:
        Agent(std::shared_ptr<model::Animat> animat);

        /// Actuate the animat based on control output
        /// Returns 0 on success, -1 if problem
        int update();

        /// Mutates the NEAT architecture
        void mutateNeat();

        /// Inherits genome from another agent
        void inheritNeat(Agent const & other);
        void inheritNeat(neat::Network const & net);

        /// Set the current central starting
        void recordStartPosition();

        void recordDistanceMoved();

        /// use start and end point to compute distance moved
        double distanceMoved() const;
        double previousDistanceMoved() const;

        void resetController();

        neat::Network getNeatNet() const;

        /// An agent is considered 'bad' if its physics
        /// became unstable during the simulation process.
        /// This is reset with age.
        void setBad();

        long getAge() const;
        void resetAge();
        void incrementAge();
        void resetNeat();

        void setAdjustedFitness(double const adjustedFitness);
        double getAdjustedFitness() const;

        void updateSpeciesColour(double r, double g, double b);
        model::SpeciesColour getSpeciesColour() const;

      private:

        /// The physical shell of the animat agent  
        std::shared_ptr<model::Animat> m_animat;

        /// The neat network that will encode the 
        /// connectivity of the controller
        neat::Network m_neat;

        /// Controls the animat agent, it's movements etc.
        std::shared_ptr<Controller> m_controller;

        /// stores start position to compute distance travelled
        physics::Vector3 m_startPosition;

        double m_distanceMoved;

        /// When physics broke
        bool mutable m_bad;

        /// rt-neat
        long m_age;

        /// Fitness is adjusted according to distance swam
        /// divided by the number of members making up the species.
        double m_adjustedFitness;

    };
}