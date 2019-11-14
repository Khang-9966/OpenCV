#ifndef UTIL_H
#define UTIL_H

#include "VBPtr.h"
#include <Matrix.h>
#include <vector>
using namespace ImageMan;
namespace Util {
    /**
     * Bilinear interp a point in a channel from an image
     */
    double interp(double x, double y, VBPtr img, int channel); 

    /**
     * Divide the elements of the matrix by the item in the bottom right
     */
    Matrix homogenize(const Matrix& rhs);

    double norm(const Matrix& mat);

    Matrix normalize(const Matrix& mat);

    /**
     * Convert a nxm matrix to a lxk where nm=kl.  Order maintained from
     * top-bottom, left-right
     */
    void convert(const Matrix& matFrom, Matrix& matTo);

    /**
     * Pick random indices of srcVals and return them
     */
    std::vector<int> pickRandVals(const std::vector<int>& srcVals, int numPick);

    template<typename T>
    T min(T a, T b) {
        return a<b?a:b; }

    /**
     * Region aware copy
     */
    void copy(VBPtr dst, VBPtr src);

    double distance(const std::vector<double>& one, const std::vector<double>& two);
    double distance(const Matrix& one, const Matrix& two);
    double normCrossCorr(VBPtr img1, VBPtr img2, int x1, int y1, int x2, int y2);
    std::pair<std::vector<Matrix>, std::vector<Matrix> >
        pilu(VBPtr img1, VBPtr img2, int numPoints);
}


#endif
