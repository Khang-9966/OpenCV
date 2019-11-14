#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <cxcore.h>
#include "imdllexp.h"
namespace ImageMan {

class IM_DLL Matrix {
private:
    Matrix(int size=CV_64FC1);
public:
    Matrix(int rows, int cols, int size=CV_64FC1);
    ~Matrix();
    Matrix(const Matrix& rhs);
    Matrix& operator=(const Matrix& rhs);

public:
    void setIdentity();
    void setData(double* data);
    double get(int row, int col) const;
    void set(int row, int col, double val);
    void set3x1(double one, double two, double three);
    int cols() const;
    int rows() const;
    double sumSqDiff(const Matrix& rhs) const;

    Matrix operator*(const Matrix& rhs) const;
    void transpose();
    double inverse();

    void SVDS(Matrix& S);
    void SVDSV(Matrix& S, Matrix& V);
    void SVDSVU(Matrix& S, Matrix& V, Matrix& U);
    void SVDSU(Matrix& S, Matrix& U);
    void SVDV(Matrix& V);
    void SVDU(Matrix& U);
    void SVDVU(Matrix& V, Matrix& U);

    friend std::ostream& operator <<(std::ostream& lhs, const Matrix& rhs) {
        using namespace std;
        for (int j=0; j<rhs.rows(); ++j) {
            for (int i=0; i<rhs.cols(); ++i)
                lhs << rhs.get(j, i) << " ";
            lhs << endl;
        }
        return lhs;
    }

    /**
     * Solve Ax = b
     * @see cvSolve
     */
    static int solve(const Matrix& a, const Matrix& b, Matrix& x, int method);

    inline operator const CvMat&() {
        return *m_mat; }
    inline operator const CvMat*() {
        return m_mat; }
private:
    CvMat* m_mat;
};
};
#endif
