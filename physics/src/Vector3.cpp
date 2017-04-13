/// Copyright (c) 2017 Ben Jones
#include <math.h>
#include "Vector3.hpp"


physics::Vector3 const physics::Vector3::Zero(0.0, 0.0, 0.0);
physics::Vector3 const physics::Vector3::X_Axis(1.0, 0.0, 0.0);
physics::Vector3 const physics::Vector3::Y_Axis(0.0, 1.0, 0.0);
physics::Vector3 const physics::Vector3::Z_Axis(0.0, 0.0, 1.0);

namespace physics {

    Vector3::Vector3(void)
    {
        set(0, 0, 0);
    }

    Vector3::Vector3(double const x, double const y, double const z)
    {
        set(x, y, z);
    }

    Vector3::Vector3(const double *v)
    {
        set(v[0], v[1], v[2]);
    }

    Vector3::Vector3(Vector3 const & v)
    {
        set(v.m_vec[0], v.m_vec[1], v.m_vec[2]);
    }

    Vector3&
    Vector3::operator=(Vector3 const & v)
    {
        set(v.m_vec[0], v.m_vec[1], v.m_vec[2]);
        return *this;
    }

    void
    Vector3::set(double const x, 
                 double const y, 
                 double const z)
    {
        m_vec[0] = x;
        m_vec[1] = y;
        m_vec[2] = z;
    }

    void Vector3::toZero()
    {
        set(0, 0, 0);
    }


    double & Vector3::operator[](int const i)
    {
        if (i < 0 || i > 2) {
            throw std::runtime_error("Vector3: out of bounds");
        }
        return m_vec[i];
    }


    double Vector3::operator[](int const i) const
    {
        if (i < 0 || i > 2) {
            throw std::runtime_error("Vector3: out of bounds");
        }
        return m_vec[i];
    }


    Vector3&
    Vector3::operator+=(const Vector3& v)
    {
        m_vec[0] += v.m_vec[0];
        m_vec[1] += v.m_vec[1];
        m_vec[2] += v.m_vec[2];
        return *this;
    }


    Vector3&
    Vector3::operator-=(const Vector3& v)
    {
        m_vec[0] -= v.m_vec[0];
        m_vec[1] -= v.m_vec[1];
        m_vec[2] -= v.m_vec[2];
        return *this;
    }


    Vector3&
    Vector3::operator*=(double const s)
    {
        m_vec[0] *= s;
        m_vec[1] *= s;
        m_vec[2] *= s;
        return *this;
    }

    Vector3&
    Vector3::operator*=(Matrix& m)
    {
        double a;
        double b;
        double c;
        a = (m_vec[0]*m[0][0])+(m_vec[1]*m[1][0])+(m_vec[2]*m[2][0]);
        b = (m_vec[0]*m[0][1])+(m_vec[1]*m[1][1])+(m_vec[2]*m[2][1]);
        c = (m_vec[0]*m[0][2])+(m_vec[1]*m[1][2])+(m_vec[2]*m[2][2]);
        m_vec[0] = a;
        m_vec[1] = b;
        m_vec[2] = c;
        return *this;
    }


    Vector3&
    Vector3::operator/=(double const s)
    {
        m_vec[0] /= s;
        m_vec[1] /= s;
        m_vec[2] /= s;
        return *this;
    }


    Vector3
    Vector3::operator+(const Vector3& v) const
    {
        return Vector3(m_vec[0]+v.m_vec[0], m_vec[1]+v.m_vec[1], m_vec[2]+v.m_vec[2]);
    }


    Vector3
    Vector3::operator-(const Vector3& v) const
    {
        return Vector3(m_vec[0]-v.m_vec[0], m_vec[1]-v.m_vec[1], m_vec[2]-v.m_vec[2]);
    }


    Vector3
    Vector3::operator-(void) const
    {
        return Vector3(-m_vec[0],-m_vec[1], -m_vec[2]);
    }


    Vector3
    Vector3::operator*(double const s) const
    {
        return Vector3(m_vec[0]*s, m_vec[1]*s, m_vec[2]*s);
    }

    Vector3
    Vector3::operator*(Matrix& m) const
    {
        double a;
        double b;
        double c;
        a = (m_vec[0]*m[0][0])+(m_vec[1]*m[1][0])+(m_vec[2]*m[2][0]);
        b = (m_vec[0]*m[0][1])+(m_vec[1]*m[1][1])+(m_vec[2]*m[2][1]);
        c = (m_vec[0]*m[0][2])+(m_vec[1]*m[1][2])+(m_vec[2]*m[2][2]);
        return Vector3(a,b,c);
    }


    Vector3
    Vector3::operator/(double const s) const
    {
        return Vector3(m_vec[0]/s, m_vec[1]/s, m_vec[2]/s);
    }


    bool
    Vector3::operator==(const Vector3& v) const
    {
        return ((m_vec[0] == v.m_vec[0]) &&
                (m_vec[1] == v.m_vec[1]) &&
                (m_vec[2] == v.m_vec[2]));
    }


    bool
    Vector3::operator!=(const Vector3& v) const
    {
        return ((m_vec[0] != v.m_vec[0]) ||
                (m_vec[1] != v.m_vec[1]) ||
                (m_vec[2] != v.m_vec[2]));
    }


    double
    Vector3::length(void) const
    {
        return sqrt(lengthSquared());
    }


    double
    Vector3::lengthSquared(void) const
    {
        return m_vec[0]*m_vec[0] + m_vec[1]*m_vec[1] + m_vec[2]*m_vec[2];
    }


    void
    Vector3::normalize(void)
    {
        double len = length();
        if (len > 0) {
            m_vec[0] /= len;
            m_vec[1] /= len;
            m_vec[2] /= len;
        }
    }

    void
    Vector3::squash(void)
    {
        m_vec[0]=tan(m_vec[0]);
        m_vec[1]=tan(m_vec[1]);
        m_vec[2]=tan(m_vec[2]);

    }

    double
    Vector3::dot(const Vector3& v) const
    {
        return m_vec[0]*v.m_vec[0] + m_vec[1]*v.m_vec[1] + m_vec[2]*v.m_vec[2];
    }


    Vector3
    Vector3::cross(const Vector3& v) const
    {
        return Vector3(m_vec[1] * v.m_vec[2] - m_vec[2] * v.m_vec[1],
                       m_vec[2] * v.m_vec[0] - m_vec[0] * v.m_vec[2],
                       m_vec[0] * v.m_vec[1] - m_vec[1] * v.m_vec[0]);
    }


    double
    Vector3::distance(const Vector3& v) const
    {
        return sqrt(distanceSquared(v));
    }


    double
    Vector3::distanceSquared(const Vector3& v) const
    {
        double dx, dy, dz;
        dx = m_vec[0] - v.m_vec[0];
        dy = m_vec[1] - v.m_vec[1];
        dz = m_vec[2] - v.m_vec[2];
        return dx*dx + dy*dy + dz*dz;
    }

    Vector3
    Vector3::multComponents(const Vector3& v) const
    {
        return Vector3(m_vec[0]*v.m_vec[0],m_vec[1]*v.m_vec[1],m_vec[2]*v.m_vec[2]);
    }

    Vector3
    Vector3::sgn() const
    {
        double sgnX, sgnY, sgnZ;

        if (m_vec[0] < 0)sgnX = -1;
        else if (m_vec[0] > 0)sgnX = 1;
        else sgnX = 0;

        if (m_vec[1] < 0)sgnY = -1;
        else if (m_vec[1] > 0)sgnY = 1;
        else sgnY = 0;

        if (m_vec[2] < 0)sgnZ = -1;
        else if (m_vec[2] > 0)sgnZ = 1;
        else sgnZ = 0;

        return Vector3(sgnX,sgnY,sgnZ);
    }

}
