/// Copyright (c) 2017 Ben Jones

#include "NeuralSubstrate.hpp"

namespace {
    void createNeuronColumn(int const segmentCount,
                            double const xCoord,
                            std::vector<std::pair<double, double> > & neuronCoords)
    {
        auto div = 2.0 / (segmentCount + 1);
        auto interval = div/2.0;
        auto yCoord = -1.0 + interval;
        for (auto i = 0; i < segmentCount; ++i) {
            neuronCoords.emplace_back(xCoord, yCoord);
            yCoord += interval;
        }
    }
}

namespace model {

    NeuralSubstrate::NeuralSubstrate(int const segmentCount)
    {

        m_neuronCoords.reserve(segmentCount * 4);

        // Left motors
        createNeuronColumn(segmentCount, -1.0, m_neuronCoords);

        // Right motors
        createNeuronColumn(segmentCount, 1.0, m_neuronCoords);

        // Left inter
        createNeuronColumn(segmentCount, -0.5, m_neuronCoords);

        // Right inter
        createNeuronColumn(segmentCount, 0.5, m_neuronCoords);
    }

    double NeuralSubstrate::getX(int const i) const
    {
        return m_neuronCoords[i].first;
    }

    double NeuralSubstrate::getY(int const i) const
    {
        return m_neuronCoords[i].second;
    }

;
}