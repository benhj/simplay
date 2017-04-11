// Very fucking hacky...

#pragma once

#include <vector>

namespace physics {

    class Matrix
    {
        using VectorMatrix = std::vector<std::vector<double> >;
      public:
        Matrix(int const w = 4, int const h = 4);
        Matrix(Matrix const &);
        void initialize();
        void toIdentity();
        void clear();
        void constructPieceWise(int const i, int const j, double const v);
        std::vector<double> & operator[](int const); // return a row
        Matrix& operator-=(Matrix&);
        Matrix& operator+=(Matrix&);
        Matrix operator-(Matrix&) const;
        Matrix operator+(Matrix&) const;
        Matrix operator*(Matrix&) const;
        Matrix operator=(Matrix&);
      private:
        int m_w;
        int m_h;
        VectorMatrix m_mat;
    };
}