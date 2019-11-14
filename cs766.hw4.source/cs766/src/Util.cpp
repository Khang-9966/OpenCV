#include "Util.h"
#include <set>
#include "Stats.h"
#include "kltWrap.h"
#include "Globals.h"
#include <algorithm>
#include <iostream>
using namespace std;
namespace Util {

    double interp(double x, double y, VBPtr img, int channel) {
        int x1 = (int)x;
        int x2 = x1+1;
        int y1 = (int)y;
        int y2 = y1+1;
        ValueBlockIterator i = img->getIterator(Region(x1, y1, 2, 2));
        int ul = i.getUChar(channel); i.inc();
        int ur = i.getUChar(channel); i.inc();
        int bl = i.getUChar(channel); i.inc();
        int br = i.getUChar(channel);
        return (1-y+y1)*((x-x1)*ur+(1-x+x1)*ul)+
            (y-y1)*((x-x1)*br+(1-x+x1)*bl);
    }

    Matrix homogenize(const Matrix& rhs) {
        double val = rhs.get(rhs.rows()-1, rhs.cols()-1);
        Matrix retVal = rhs;
        for (int i=0; i<retVal.rows(); ++i)
            for (int j=0; j<retVal.cols(); ++j)
                retVal.set(i, j, retVal.get(i, j)/val);
        retVal.set(rhs.rows()-1, rhs.cols()-1, 1);
        return retVal;
    }

    void convert(const Matrix& matFrom, Matrix& matTo) {
        if (matFrom.cols()*matFrom.rows() > matTo.cols()*matTo.rows())
            IMERR_THROW("Cannot fit from matrix into to matrix");
        int numVals = matFrom.cols()*matFrom.rows();
        for (int i=0; i<numVals; ++i)
            matTo.set(i/matTo.cols(), i%matTo.cols(),
                matFrom.get(i/matFrom.cols(), i%matFrom.cols()));
    }

    std::vector<int> pickRandVals(const std::vector<int>& srcVals, int numPick) {
        if ((int)srcVals.size() <= numPick)
            return srcVals;
        std::vector<int> retVal;
        std::set<int> ptSet;
        while (ptSet.size() != numPick) {
            int nextIndex = (int)Stats::randVal(0, srcVals.size()-1);
            ptSet.insert(srcVals[nextIndex]);
        }
        std::set<int>::iterator iSet = ptSet.begin();
        for (; iSet != ptSet.end(); ++iSet)
            retVal.push_back(*iSet);
        return retVal;
    }

    void copy(VBPtr dst, VBPtr src) {
        if (dst->getNumChannels() != src->getNumChannels())
            IMERR_THROW("Channels don't match");
        int channels = dst->getNumChannels();
        Region copyRegion = dst->getRegion().intersectWith(src->getRegion());
        ValueBlockIterator iDst = dst->getIterator(copyRegion);
        ValueBlockIterator iSrc = src->getIterator(copyRegion);
        for (; !iDst.passedRegionEnd(); iDst.inc(), iSrc.inc()) {
            for (int i=0; i<channels; ++i)
                iDst.set(iSrc.getUChar(i), i);
        }
    }

    double norm(const Matrix& mat) {
        double retVal = 0;
        for (int i=0; i<mat.rows(); ++i)
            for (int j=0; j<mat.cols(); ++j)
                retVal += pow(mat.get(i,j),2);
        return sqrt(retVal);
    }

    Matrix normalize(const Matrix& mat) {
        Matrix retVal = mat;
        double val = norm(mat);
        for (int i=0; i<mat.rows(); ++i)
            for (int j=0; j<mat.cols(); ++j)
                retVal.set(i, j, retVal.get(i, j)/val);
        return retVal;
    }

    double distance(const std::vector<double>& one, const std::vector<double>& two) {
        if (one.size() != two.size()) IMERR_WARN("Cannot distance different dimensions");
        double val = 0;
        std::vector<double>::const_iterator a = one.begin();
        std::vector<double>::const_iterator b = two.begin();
        for (; a != one.end(); ++a, ++b)
            val += pow(*a-*b, 2);
        return sqrt(val);
    }

    double distance(const Matrix& one, const Matrix& two) {
        if (one.rows() != two.rows() && one.cols() != two.cols())
            IMERR_WARN("Cannot distance different dimensions");
        double val = 0;
        for (int j=0; j<one.rows(); ++j)
            for (int i=0; i<one.cols(); ++i) {
                val += pow(one.get(j,i)-two.get(j,i), 2);
            }
        return sqrt(val);
    }

    double normCrossCorr(VBPtr img1, VBPtr img2, int x1, int y1, int x2, int y2, int window,
        CvScalar ave1, CvScalar ave2, CvScalar std1, CvScalar std2) {
        double num = 0;
        for (int i=0; i<window; ++i) {
            for (int j=0; j<window; ++j) {
                int indY1 = j+y1-(window/2);
                int indX1 = i+x1-(window/2);
                int indY2 = j+y2-(window/2);
                int indX2 = i+x2-(window/2);
                unsigned char a = ((unsigned char*)img1->getRow(indY1))[indX1];
                unsigned char b = ((unsigned char*)img2->getRow(indY2))[indX2];
                num += (a-ave1.val[0])*(b-ave2.val[0]);
            }
        }
        double val = num / (window*window*std1.val[0]*std2.val[0]);
        val = val > -1 ? val : -1;
        val = val < 1 ? val : 1;
        return val;
    }

    class SortMe {
    public:
        SortMe(double val, int i, int j) : m_val(val), m_i(i), m_j(j) {}
        bool operator<(const SortMe& rhs) const {
            return m_val > rhs.m_val; }
        double m_val;
        int m_i, m_j;
    };

    std::pair<std::vector<Matrix>, std::vector<Matrix> >
        pilu(VBPtr img1, VBPtr img2, int numPoints)
    {
        KLT_FeatureList fl1 = getFeatures(img1, Globals::NUM_FEATS);
        KLT_FeatureList fl2 = getFeatures(img2, Globals::NUM_FEATS);
        std::vector<int> good1 = goodIndices(fl1);
        std::vector<int> good2 = goodIndices(fl2);
        int M = (int)good1.size();
        int N = (int)good2.size();
        Matrix G(M,N);
        CvScalar ave1, std1, ave2, std2;
        cvAvgSdv(img1.get(), &ave1, &std1);
        cvAvgSdv(img2.get(), &ave2, &std2);
        for (int i=0; i<M; ++i) {
            for (int j=0; j<N; ++j) {
                int x1 = (int)fl1->feature[good1[i]]->x;
                int y1 = (int)fl1->feature[good1[i]]->y;
                int x2 = (int)fl2->feature[good2[j]]->x;
                int y2 = (int)fl2->feature[good2[j]]->y;
                double C = normCrossCorr(img1, img2, x1, y1, x2, y2, Globals::PILU_WIN_SIZE,
                    ave1, ave2, std1, std2);
                double dist = sqrt(pow(x1-x2,2)+pow(y1-y2,2));
                if (x1==x2 && y1==y2)
                    G.set(i, j, 0);
                else {
                    double val = (C+1)/2*exp(-pow(dist,2)/(2*pow(Globals::PILU_SIGMA,2)));
//                    cout << -pow(dist,2)/(2*pow(Globals::PILU_SIGMA,2)) << " ";
//                    cout << C << ":" << val << endl;
                    G.set(i, j, val);
                }
            }
        }
//        cout << G << endl;
        Matrix T((int)good1.size(), (int)good1.size()),
            D((int)good1.size(), (int)good2.size()),
            U((int)good2.size(), (int)good2.size());
        cvSVD(G, D, T, U, CV_SVD_V_T);
        int minSize = (int)min(good1.size(), good2.size());
        for (int i=0; i<minSize; ++i) {
            D.set(i,i,1);
        }
//        D.set(good1.size()-1, good2.size()-1, 0);
        Matrix P = T*D*U;
        // Now pick out the best
        int* maxIndexI = new int[(int)good1.size()];
        int* maxIndexJ = new int[(int)good2.size()];
        for (int i=0; i<(int)good1.size(); ++i) {
            double bestVal = 0;
            int bestIdx = -1;
            for (int j=0; j<(int)good2.size(); ++j) {
                if (P.get(i, j) > bestVal) {
                    bestVal = P.get(i,j);
                    bestIdx = j;
                }
            }
            maxIndexI[i] = bestIdx;
        }
        for (int j=0; j<(int)good2.size(); ++j) {
            double bestVal = 0;
            int bestIdx = -1;
            for (int i=0; i<(int)good1.size(); ++i) {
                if (P.get(i, j) > bestVal) {
                    bestVal = P.get(i,j);
                    bestIdx = i;
                }
            }
            maxIndexJ[j] = bestIdx;
        }
 
        std::vector<SortMe> sort;
        for (int i=0; i<(int)good1.size(); ++i) {
            for (int j=0; j<(int)good2.size(); ++j) {
                if (maxIndexJ[maxIndexI[i]] == i) {
                    sort.push_back(SortMe(P.get(i,j), i, j));
                }
            }
        }
        std::sort(sort.begin(), sort.end());

        std::pair<std::vector<Matrix>, std::vector<Matrix> >
            retVal = make_pair(std::vector<Matrix>(), std::vector<Matrix>());
        retVal.first.resize(numPoints, Matrix(2,1));
        retVal.second.resize(numPoints, Matrix(2,1));
        for (int i=0; i<numPoints; ++i) {
            KLT_Feature f1 = fl1->feature[good1[sort[i].m_i]];
            KLT_Feature f2 = fl2->feature[good2[sort[i].m_j]];
            retVal.first[i].set(0,0, f1->x);
            retVal.first[i].set(1,0, f1->y);
            retVal.second[i].set(0,0, f2->x);
            retVal.second[i].set(1,0, f2->y);
            //cout << sort[i].m_i << " " << sort[i].m_j << " " << sort[i].m_val << endl;
        }
        return retVal;
    }

};