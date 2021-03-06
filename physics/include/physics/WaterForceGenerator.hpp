/// Copyright (c) 2017 Ben Jones
#pragma once

#include "model/AnimatLayer.hpp"
#include "PhysicsEngine.hpp"
#include "Vector3.hpp"

namespace physics {
  class WaterForceGenerator
  {
    public:
      WaterForceGenerator(model::AnimatLayer & layerOne,
                          model::AnimatLayer & layerTwo,
                          PhysicsEngine & physicsEngine);
      void apply();

    private:
      Vector3 computeFaceNormalLeft() const;
      Vector3 computeFaceNormalRight() const;
      double computeLeftSideArea() const;
      double computeRightSideArea() const;
      Vector3 getTangentLeft() const;
      Vector3 getTangentRight() const;
      double sgn(double const in) const;
  
      model::AnimatLayer & m_layerOne;
      model::AnimatLayer & m_layerTwo;
      PhysicsEngine & m_physicsEngine;

  };
}
