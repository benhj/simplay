/// Copyright (c) 2017 Ben Jones
#pragma once

/**
 * Based on code found at http://resumbrae.com/ub/dms424_s03/
 */

#include "PointMass.hpp"

namespace physics {

    class Spring
    {
      public:

        Spring(PointMass & p0, 
               PointMass & p1, 
               double const k = 1.0,
               double const dampener = 0.9);

        void apply(void);
        void setFixedPoints(bool const, bool const);
        inline const PointMass& point0(void) { return m_p0; }
        inline const PointMass& point1(void) { return m_p1; }
        inline double restLength(void) const { return m_restLength; }
        inline double springConstant(void) const { return m_springConstant; }
        void setRestLength(double const len);
        void setSpringConstant(double const k);
        void setDampener(double const d);
        double getCurrentDistension();
        void compress(double const forceMagnitude);
        void relax();

      private:
        PointMass & m_p0;
        PointMass & m_p1;
        double m_springConstant;
        double m_dampener;
        double m_restLength;
        bool m_fixedP0;
        bool m_fixedP1;
        Vector3 m_compressForceP0;
        Vector3 m_compressForceP1;
    };

}
