#ifndef EPIGEOM_H
#define EPIGEOM_H

#include <ipl.h>
#include <Matrix.h>
#include <vector>
using namespace ImageMan;
class EpiGeom {
public:
    EpiGeom();

    /**
     * Initialize with a pre-determined fundamental matrix
     */
    EpiGeom(const Matrix& fundMatrix);

    /**
     * Use SVD to determine the fundamental matrix from a list of point
     * correspondences
     */
    EpiGeom(std::vector<Matrix>& ptsL, std::vector<Matrix>& ptsR);

    /**
     * Given a 3x1 pt in the left image, find the 3x1 line parameters ax+by+c = 0
     */
    Matrix getLineParamsR(const Matrix& leftPt);

    /**
     * Given a 3x1 pt in the right image, find the 3x1 line parameters ax+by+c = 0
     */
    Matrix getLineParamsL(const Matrix& rightPt);

    inline Matrix getFund() {
        return m_fund; }

    Matrix getEpipoleL();
    Matrix getEpipoleR();
private:
    Matrix m_fund;
};

#endif
