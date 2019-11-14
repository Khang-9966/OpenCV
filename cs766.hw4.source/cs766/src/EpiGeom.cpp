#include "EpiGeom.h"
#include <debug.h>
#include "Util.h"
#include <iostream>
#include <cxcore.h>
#include <cv.h>
using namespace std;
EpiGeom::EpiGeom() : m_fund(3,3) {}

EpiGeom::EpiGeom(const Matrix& fundMatrix) : m_fund(fundMatrix) {
}

EpiGeom::EpiGeom(std::vector<Matrix>& ptsL, std::vector<Matrix>& ptsR) :
    m_fund(3,3)
{
/*    // So A is 
    //[x1x2 x1y2 x1w2 y1x2 y1y2 y1w2 w1x2 w1y2 1]
    // with one row per point correspondence.  Then we do the SVD of
    // A^TA and take the least eigenvector for f.  Then adjust so the
    // norm is 1

    int numPts = (int)Util::min(ptsL.size(), ptsR.size());

    Matrix a(numPts,9);

    std::vector<Matrix>::const_iterator iPt1 = ptsL.begin(), iPt2 = ptsR.begin();
    
    for (int i=0; i<numPts; ++i, ++iPt1, ++iPt2) {
//        cout << "*" << endl << ptsL[i] << endl << *iPt2 << endl;
        a.set(i, 0, iPt2->get(0,0)*iPt1->get(0,0));
        a.set(i, 1, iPt2->get(0,0)*iPt1->get(1,0));
        a.set(i, 2, iPt2->get(0,0));
        a.set(i, 3, iPt2->get(1,0)*iPt1->get(0,0));
        a.set(i, 4, iPt2->get(1,0)*iPt1->get(1,0));
        a.set(i, 5, iPt2->get(1,0));
        a.set(i, 6, iPt1->get(0,0));
        a.set(i, 7, iPt1->get(1,0));
        a.set(i, 8, 1);
    }
    Matrix w(9,1);
    Matrix v(9,9);
    // a is a numPtsx9
    cvSVD(a, w, 0, v);
    for (int i=0; i<9; ++i)
        m_fund.set(i/3, i%3, v.get(i,8));
    m_fund = Util::normalize(m_fund);
//    Util::normalize(m_fund);
*/
    // Now adjust as Hartley does
/*    Matrix u(3,3);
    w = Matrix(3,3);
    v = Matrix(3,3);
    cvSVD(m_fund, w, u, v, CV_SVD_V_T);
    w.set(0,1,0);
    w.set(0,2,0);
    w.set(1,0,0);
    w.set(1,2,0);
    w.set(2,0,0);
    w.set(2,1,0);
    w.set(2,2,0);
    m_fund = u*w*v;*/
//    cout << "fund:" << endl;
//    cout << m_fund << endl;
    int numPts = (int)Util::min(ptsL.size(), ptsR.size());
    Matrix cvPts1(2, numPts);
    Matrix cvPts2(2, numPts);
    for (int i=0; i<numPts; ++i) {
        cvPts1.set(0, i, ptsL[i].get(0, 0));
        cvPts1.set(1, i, ptsL[i].get(1, 0));
        cvPts2.set(0, i, ptsR[i].get(0, 0));
        cvPts2.set(1, i, ptsR[i].get(1, 0));
    }
    cvFindFundamentalMat(cvPts1, cvPts2, m_fund, CV_FM_8POINT, .5, .99);
}

Matrix EpiGeom::getLineParamsR(const Matrix& leftPt) {
    Matrix temp(3,1);
    Util::convert(leftPt, temp);
    temp = m_fund*temp;
    return temp;
}

Matrix EpiGeom::getLineParamsL(const Matrix& rightPt) {
    Matrix temp(1,3);
    Util::convert(rightPt, temp);
    temp = temp*m_fund;
    Matrix ret(3,1);
    Util::convert(temp, ret);
    return ret;
}

Matrix EpiGeom::getEpipoleL() {
    // Fm = 0
    Matrix retVal(3,1);
    Matrix b(3,1);
    b.set(2,0,1);
    int debug = Matrix::solve(m_fund, b, retVal, CV_SVD);
    return retVal;
}

Matrix EpiGeom::getEpipoleR() {
    // FTm = 0
    Matrix A(m_fund);
    A.transpose();
    Matrix retVal(3,1);
    Matrix b(3,1);
    b.set(2,0,1);
    Matrix::solve(A, b, retVal, CV_SVD);
    return retVal;
}

