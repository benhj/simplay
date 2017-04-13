/// Copyright (c) 2017 Ben Jones

#include "Matrix.hpp"
#include <iostream>

namespace physics {

    Matrix::Matrix(int const w, int const h)
      : m_w(w)
      , m_h(h)
    {
        initialize();
    }

    Matrix::Matrix(Matrix const & m)
      : m_w(m.m_w)
      , m_h(m.m_h)
      , m_mat(m.m_mat)
    {
    }

    void
    Matrix::initialize()
    {
        for (auto i = 0; i < m_h; ++i) {
            std::vector<double> inner;
            inner.assign(m_w, 0);
            m_mat.push_back(inner);
        }
    }

    void
    Matrix::constructPieceWise(int const i, 
                               int const j, 
                               double const v)
    {
        m_mat[i][j] = v;
    }

    void
    Matrix::toIdentity()
    {
        for (auto i = 0; i < m_h; ++i) {
            m_mat[i][i] = 1.0;
        }
    }

    void
    Matrix::clear()
    {
        std::vector<std::vector<double > >::iterator it;
        for (auto it = std::begin(m_mat); it != std::end(m_mat); ++it) {
            std::vector<double>::iterator itb;
            for (auto itb = std::begin(*it); itb != std::end(*it); ++itb) {
                *itb = 0.0;
            }
        }
    }

    std::vector<double> &
    Matrix::operator[](int const i)
    {
        return m_mat[i];
    }

    Matrix&
    Matrix::operator-=(Matrix & m_)
    {
        VectorMatrix::iterator it;
        for (size_t i = 0 ; i < m_mat.size(); ++i) {
            for (size_t j = 0; j < m_mat[i].size(); ++j) {
                m_mat[i][j] -= m_[i][j];
            }
        }
        return *this;
    }

    Matrix&
    Matrix::operator+=(Matrix & m_)
    {
        for (size_t i = 0 ; i < m_mat.size(); ++i) {
            for (size_t j = 0; j < m_mat[i].size(); ++j) {
                m_mat[i][j] += m_[i][j];
            }
        }
        return *this;
    }


    Matrix
    Matrix::operator-(Matrix & m_) const
    {
        Matrix toReturn(m_w, m_h);
        toReturn.initialize();
        for (size_t i = 0 ; i < m_mat.size(); ++i) {
            for (size_t j = 0; j < m_mat[i].size(); ++j) {
                toReturn[i][j] = m_mat[i][j] - m_[i][j];
            }
        }
        return toReturn;

    }
    Matrix
    Matrix::operator+(Matrix & m_) const
    {
        Matrix toReturn(m_w, m_h);
        toReturn.initialize();
        for (size_t i = 0 ; i < m_mat.size(); ++i) {
            for (size_t j = 0; j < m_mat[i].size(); ++j) {
                toReturn[i][j] = m_mat[i][j] + m_[i][j];
            }
        }
        return toReturn;

    }
    Matrix
    Matrix::operator*(Matrix& m_) const
    {
        Matrix compositeMatrix(m_w, m_h);
        double compSum = 0;
        for (int i = 0; i < m_h; ++i) {
            for (int j = 0; j < m_w; ++j) {
                compSum = 0;
                for (int k = 0; k < m_w; ++k) {
                    compSum += (double const)(m_mat[i][k] * m_[k][j]);
                }
                compositeMatrix[i][j] = compSum;
            }
        }

        return compositeMatrix;
    }

    Matrix
    Matrix::operator=(Matrix& m_)
    {
        for (int i = 0; i < m_h; ++i) {
            m_mat[i] = m_[i];
        }
        return *this;
    }
}
