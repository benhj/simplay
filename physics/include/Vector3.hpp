
#pragma once

/**
 *
 * Based on code found at http://resumbrae.com/ub/dms424_s03/libdms/
 *
 */

#include "Matrix.hpp"
#include <iostream>

namespace physics {

    class Vector3
    {
      public:

        double m_vec[3];
        Vector3(void);
        Vector3(double const x, double const y, double const z);
        Vector3(const double *v);
        Vector3(const Vector3 &v);
        Vector3& operator= (const Vector3& v);
        void set(double const x, double const y, double const z);

        void toZero();

        double& operator[](int const i);
        double operator[](int const i) const;

        Vector3& operator+=(const Vector3& v);
        Vector3& operator-=(const Vector3& v);
        Vector3& operator*=(double const s);
        Vector3& operator*=(Matrix& m);
        Vector3& operator/=(double const s);
        Vector3 operator+(const Vector3& v) const;
        Vector3 operator-(const Vector3& v) const;
        Vector3 operator*(double const s) const;
        Vector3 operator*(Matrix& m) const;
        Vector3 operator/(double const s) const;
        Vector3 operator-(void) const;
        bool operator==(const Vector3& v) const;
        bool operator!=(const Vector3& v) const;

        double length(void) const;
        double lengthSquared(void) const;
        void normalize(void);
        void squash(void);
        double dot(const Vector3& v) const;
        Vector3 cross(const Vector3& v) const;
        Vector3 multComponents(const Vector3& v) const;
        double distance(const Vector3& v) const;
        double distanceSquared(const Vector3& v) const;
        Vector3 sgn(void) const;
        static const Vector3 Zero;
        static const Vector3 X_Axis;
        static const Vector3 Y_Axis;
        static const Vector3 Z_Axis;
    };

}

inline std::ostream& operator<< (std::ostream& s, physics::Vector3 const & v)
{ return s << "(" << v.m_vec[0] << ", " << v.m_vec[1] << ", " << v.m_vec[2] << ")"; }

inline physics::Vector3 operator*(double const s, 
                                  physics::Vector3 const & v) { return v*s; }

