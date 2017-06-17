/// Copyright (c) 2017 Ben Jones

#pragma once

#include <vector>

namespace model {
    class NeuralSubstrate
    {
      public:
        explicit NeuralSubstrate(int const sgementCount);
        NeuralSubstrate() = delete;

        double getX(int const i) const;
        double getY(int const i) const;

      private:

        /// The neuron coordinate locations
        std::vector<std::pair<double, double> > m_neuronCoords;
    };
}