
#include "Animat.hpp"
#include "Spring.hpp"
#include "PointMass.hpp"
#include "Vector3.hpp"

namespace model {

    Animat3D::Animat3D(int const layers,
                       double const layerWidth,
                       double const blockHeight)
      : m_physicsEngine()
    {
        // construct layers
        auto yOffset = blockHeight;
        auto xOffset = 0;
        for (int layer = 0; layer < layers; ++layer) {
            m_layers.emplace_back(xOffset, yOffset, layerWidth, m_physicsEngine);
            yOffset += blockHeight;
        }

        // construct blocks
        for (int block = 0; block < layers - 1; ++block) {
            m_blocks.emplace_back(m_layers[block], 
                                  m_layers[block + 1],
                                  m_physicsEngine);
        }
    }

    void
    Animat3D::applyBlockContraction(int const block, 
                                    int const side, 
                                    double const force)
    {
        if (side == 0) {
            m_blocks[block].contractLeftSide(m_physicsEngine, force);
        } else {
            m_blocks[block].contractRightSide(m_physicsEngine, force);
        }
    }

    void
    Animat3D::update()
    {
        m_physicsEngine.update(0.1);
    }
}
