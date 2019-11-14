#include "Align.h"
#include "kltWrap.h"
#include "HierCluster.h"
using namespace ImageMan;
using namespace std;
//#include <vector>
//#include <algorithm>
//#include <iostream>
//#include <stdlib.h>
#include "Util.h"
#include "globals.h"

/**
 * Get the median x, y translation as determined by tracked features
 * @return 2x1 matrix
 */
Matrix getMedianTranslation(VBPtr from, VBPtr to) {
    Matrix retVal(2,1);
    pair<KLT_FeatureList, KLT_FeatureList> f = getFeatures(from, to, Globals::NUM_FEATS);
    vector<double> x, y;

    for (int i=0; i<f.first->nFeatures; ++i) {
        if (f.first->feature[i]->val!=KLT_NOT_FOUND && f.second->feature[i]->val == KLT_TRACKED) {
            x.push_back(f.second->feature[i]->x-f.first->feature[i]->x);
            y.push_back(f.second->feature[i]->y-f.second->feature[i]->y);
        }
    }
    int s = (int)x.size();
    if (s%2) {
        // Odd, so just return median
        retVal.set(0,0,x[s/2]);
        retVal.set(1,0,y[s/2]);
    } else {
        // Even, so average of two options
        retVal.set(0,0,(x[s/2]+x[s/2-1])/2.0);
        retVal.set(1,0,(y[s/2]+y[s/2-1])/2.0);
    }
    return retVal;
}

/**
 * Get the average x, y translation determined by tracked features
 * @return 2x1 matrix
 */
Matrix getAverageTranslation(VBPtr from, VBPtr to) {
    Matrix retVal(2,1);
    pair<KLT_FeatureList, KLT_FeatureList> f = getFeatures(from, to, Globals::NUM_FEATS);
    double numFeatures = 0.0;

    for (int i=0; i<f.first->nFeatures; ++i) {
        if (f.first->feature[i]->val!=KLT_NOT_FOUND && f.second->feature[i]->val == KLT_TRACKED) {
            ++numFeatures;
            cout << f.second->feature[i]->x-f.first->feature[i]->x << endl;
            retVal.set(0,0,retVal.get(0,0)+f.second->feature[i]->x-f.first->feature[i]->x);
            retVal.set(1,0,retVal.get(1,0)+f.second->feature[i]->y-f.first->feature[i]->y);
        }
    }
    retVal.set(0,0,retVal.get(0,0)/numFeatures);
    retVal.set(1,0,retVal.get(1,0)/numFeatures);
    return retVal;
}

LargeInt score(const Matrix& transform, VBPtr from, VBPtr to) {
    LargeInt retVal(0);
    Matrix pt(3,1);
    ValueBlockIterator i = from->getIterator();
    for (; !i.passedRegionEnd(); i.inc()) {
        pt.set3x1(i.getX(), i.getY(), 1);
        pt = transform*pt;
        pt = Util::homogenize(pt);
        for (int j=0; j<i.numChannels(); ++j) {
            double val = Util::interp(pt.get(0,0), pt.get(1, 0), to, j);
            retVal += (int)((val-i.getUChar(j))*(val-i.getUChar(j)));
        }
        for (int j=0; j<100; ++j)
            i.inc();
    }
    return retVal;
}

double scoreF4(Matrix fund, const std::pair<KLT_FeatureList, KLT_FeatureList>& tracked) {
    std::vector<int> good = goodIndices(tracked);
    static Matrix pt1(1,3);
    static Matrix pt2(3,1);
    double score=0;
    for (int i=0; i<(int)good.size(); ++i) {
        KLT_Feature f1 = tracked.first->feature[good[i]];
        KLT_Feature f2 = tracked.second->feature[good[i]];
        pt1.set(0,0,f1->x); pt1.set(0,1,f1->y); pt1.set(0,2,1);
        pt2.set3x1(f2->x, f2->y, 1);
        Matrix val = pt1*fund*pt2;
//        cout << val.get(0,0) << endl;
        score += pow(val.get(0,0),2);
    }
//    cout << numMatch << endl;
    return score;
}

double scoreF3(Matrix fund, KLT_FeatureList fl1, KLT_FeatureList fl2) {
    static Matrix pt1(1,3);
    static Matrix pt2(3,1);
    const static double DIST = .1;
    std::vector<int> good1 = goodIndices(fl1),
        good2 = goodIndices(fl2);
    int numMatch = 0;
    for (int i=0; i<(int)good1.size(); ++i) {
        KLT_Feature f1 = fl1->feature[good1[i]];
        pt1.set(0,0,f1->x); pt1.set(0,1,f1->y); pt1.set(0,2,1);
        Matrix func = pt1*fund;
        for (int j=0; j<(int)good2.size(); ++j) {
            KLT_Feature f2 = fl2->feature[good2[j]];
            pt2.set3x1(f2->x, f2->y, 1);
            Matrix val = func*pt2;
            if (pow(val.get(0,0),2) < DIST) {
                ++numMatch;
                break;
            }
        }
    }
//    cout << numMatch << endl;
    return numMatch;
}

double scoreF(Matrix fund, KLT_FeatureList fl1, KLT_FeatureList fl2) {
    const static double THRESH = .1;
    static Matrix pt1(1,3);
    static Matrix pt2(3,1);
    double score = 0;
    int stop = min(fl1->nFeatures, 100);
    for (int i=0; i<stop; ++i) {
        KLT_Feature f1 = fl1->feature[i];
        if (f1->val != KLT_NOT_FOUND) {
            pt1.set(0,0,f1->x); pt1.set(0,1,f1->y); pt1.set(0,2,1);
            Matrix func = pt1*fund;
            double bestScore = 0;
            int stop2 = min(fl2->nFeatures, 100);
            for (int j=0; j<stop2; ++j) {
                KLT_Feature f2 = fl2->feature[j];
                if (f2->val != KLT_NOT_FOUND) {
                    pt2.set3x1(f2->x, f2->y, 1);
                    Matrix val = func*pt2;
                    double thisScore = (THRESH-min(abs(val.get(0,0)), THRESH))/THRESH;
//                    cout << val.get(0,0) << " " << thisScore << endl;
                    if (thisScore > bestScore)
                        bestScore = thisScore;
//                    cout << thisScore << " : " << val.get(0,0) << " : " << bestScore << endl;
                }
            }
            score += bestScore;
        }
    }
    return score;
}

EpiGeom getFundamentalMatrix2(VBPtr img1, VBPtr img2) {
    int numPts = Globals::NUM_PTS;
    KLT_FeatureList fl1 = getFeatures(img1, Globals::NUM_FEATS);
    KLT_FeatureList fl2 = getFeatures(img2, Globals::NUM_FEATS);
    std::vector<int> good1 = goodIndices(fl1),
        good2 = goodIndices(fl2);
    numPts = Util::min<int>(numPts, (int)good1.size());
    numPts = Util::min<int>(numPts, (int)good2.size());

    EpiGeom retVal;
    double bestScore = 0;
    std::vector<std::vector<double> > clusterMe1;
    std::vector<std::vector<double> > clusterMe2;
    clusterMe1.resize(numPts);
    clusterMe2.resize(numPts);
    for (int i=0; i<numPts; ++i) {
        std::vector<double> temp1, temp2;
        clusterMe1[i].push_back(fl1->feature[good1[i]]->x);
        clusterMe1[i].push_back(fl1->feature[good1[i]]->y);
        clusterMe2[i].push_back(fl2->feature[good2[i]]->x);
        clusterMe2[i].push_back(fl2->feature[good2[i]]->y);
    }

    HierCluster hc1(clusterMe1, HierCluster::SINGLE_LINK);
    HierCluster hc2(clusterMe2, HierCluster::SINGLE_LINK);

    for (int i=0; i<Globals::NUM_ITER; ++i) {
        std::vector<int> rand1 = Util::pickRandVals(good1, 8);
        std::vector<int> rand2 = Util::pickRandVals(good2, 8);



    }

/*    Matrix testVal(3,3);
    testVal.set(2,2,1);

    Matrix x(8,1);
    Matrix b(NUM_PTS, 1);
    for (int i=0; i<NUM_PTS; ++i)
        b.set(i,0,-1);
    Matrix a(NUM_PTS,8);
    double bestScore = 0;

    for (int q=0; q<NUM_ITER; ++q) {
        std::vector<int> pts1 = Util::pickRandVals(good1, NUM_PTS),
            pts2 = Util::pickRandVals(good2, NUM_PTS);

        std::vector<Matrix> pass1;
        std::vector<Matrix> pass2;
        Matrix temp(2,1);
        for (int i=0; i<(int)pts1.size(); ++i) {
            temp.set(0,0,fl1->feature[pts1[i]]->x);
            temp.set(1,0,fl1->feature[pts1[i]]->y);
            pass1.push_back(temp);
        }
        for (int i=0; i<(int)pts2.size(); ++i) {
            temp.set(0,0,fl2->feature[pts2[i]]->x);
            temp.set(1,0,fl2->feature[pts2[i]]->y);
            pass2.push_back(temp);
        }
        EpiGeom thisPass(pass1, pass2);
        double thisScore = scoreF(thisPass.getFund(), fl1, fl2);
        if (q == 0 || thisScore > bestScore) {
            retVal = thisPass;
            bestScore = thisScore;
        }
    }
*/
    KLTFreeFeatureList(fl1);
    KLTFreeFeatureList(fl2);
    return retVal;
}

EpiGeom computeEpiGeom(const std::vector<int>& pts,
                       const std::pair<KLT_FeatureList, KLT_FeatureList>& feat)
{
    std::vector<Matrix> pass1, pass2;
    static Matrix temp(2,1);
    for (int i=0; i<(int)pts.size(); ++i) {
        temp.set(0,0,feat.first->feature[pts[i]]->x);
        temp.set(1,0,feat.first->feature[pts[i]]->y);
        pass1.push_back(temp);
        temp.set(0,0,feat.second->feature[pts[i]]->x);
        temp.set(1,0,feat.second->feature[pts[i]]->y);
        pass2.push_back(temp);
    }
    return EpiGeom(pass1, pass2);
}

EpiGeom computeEpiGeomMinus(const std::vector<int>& pts,
                       const std::pair<KLT_FeatureList, KLT_FeatureList>& feat,
                       int leaveOut)
{
    std::vector<Matrix> pass1, pass2;
    static Matrix temp(2,1);
    for (int i=0; i<(int)pts.size(); ++i) {
        if (i != leaveOut) {
            temp.set(0,0,feat.first->feature[pts[i]]->x);
            temp.set(1,0,feat.first->feature[pts[i]]->y);
            pass1.push_back(temp);
            temp.set(0,0,feat.second->feature[pts[i]]->x);
            temp.set(1,0,feat.second->feature[pts[i]]->y);
            pass2.push_back(temp);
        }
    }
    return EpiGeom(pass1, pass2);
}

EpiGeom getFundamentalMatrix4(VBPtr img1, VBPtr img2) {
    std::pair<KLT_FeatureList, KLT_FeatureList> feat = 
        getFeatures(img1, img2, Globals::NUM_FEATS);
    std::vector<int> good = goodIndices(feat);

    double bestScore = 0;
    EpiGeom retVal;
    std::vector<int> bestVals;
    for (int i=0; i<Globals::NUM_ITER; ++i) {
        std::vector<int> rand = Util::pickRandVals(good, Globals::NUM_PTS);
        EpiGeom temp = computeEpiGeom(rand, feat);
        double thisScore = 0;
        for (int j=0; j<rand.size(); ++j) {
            KLT_Feature fl = feat.first->feature[j];
            KLT_Feature fr = feat.second->feature[j];
            Matrix leftPt(3,1);
            leftPt.set3x1(fl->x, fl->y, 1);
            Matrix lineR = temp.getLineParamsR(leftPt);
            thisScore += abs(lineR.get(0,0)*fr->x+lineR.get(1,0)*fr->y+lineR.get(2,0));
        }
        if (thisScore < bestScore || i==0) {
            retVal = temp;
            bestScore = thisScore;
            bestVals = rand;
        }
    }
/*    EpiGeom retVal = computeEpiGeom(good, feat);
    double change;
    cout << good.size() << endl;
    do {
        cout << "----" << endl;
        int worstIndex = 0;
        EpiGeom temp = computeEpiGeomMinus(good, feat, 0);
        Matrix origin(3,1);
        origin.set3x1(0, 0,1);

        change = pow((feat.first->feature[good[0]]->y-
                  feat.second->feature[good[0]]->y),2)+
                 pow((feat.first->feature[good[0]]->x-
                  feat.second->feature[good[0]]->x),2);
//        change = Util::distance(Util::homogenize(temp.getLineParamsL(origin)),
//                    Util::homogenize(retVal.getLineParamsR(origin)));
        for (int i=1; i<good.size(); ++i) {
            temp = computeEpiGeomMinus(good, feat, i);
            double thisCost = 
                 pow((feat.first->feature[good[i]]->y-
                  feat.second->feature[good[i]]->y),2)+
                 pow((feat.first->feature[good[i]]->x-
                  feat.second->feature[good[i]]->x),2);
            if (thisCost > change && thisCost != 0 || change != change) {
                worstIndex = i;
                change = thisCost;
            }
        }
        good.erase(good.begin()+worstIndex);
//        retVal = computeEpiGeom(good, feat);
    } while (good.size() > 10 && change > .1);
    cout << good.size() << " " << change << endl;

    EpiGeom temp = computeEpiGeom(Util::pickRandVals(good, 10), feat);
    cout << " : " << Util::distance(Util::homogenize(temp.getEpipoleL()),
        Util::homogenize(retVal.getEpipoleR())) << endl;
    retVal = computeEpiGeom(good, feat);
//    retVal = temp;*/
    cout << bestScore << endl;
    for (int i=0; i<bestVals.size(); ++i) {
        cout << (feat.first->feature[bestVals[i]]->y)-
                (feat.second->feature[bestVals[i]]->y) << endl;

    }
    KLTFreeFeatureList(feat.first);
    KLTFreeFeatureList(feat.second);
    return retVal;
}

EpiGeom getFundamentalMatrix3(VBPtr img1, VBPtr img2) {
    int NUM_PTS = 8;
    KLT_FeatureList fl1 = getFeatures(img1, Globals::NUM_FEATS);
    KLT_FeatureList fl2 = getFeatures(img2, Globals::NUM_FEATS);
    std::vector<int> good1 = goodIndices(fl1),
        good2 = goodIndices(fl2);
    NUM_PTS = Util::min<int>(NUM_PTS, (int)good1.size());
    NUM_PTS = Util::min<int>(NUM_PTS, (int)good2.size());

    EpiGeom retVal;
    double bestScore = 0;

    for (int q=0; q<Globals::NUM_ITER; ++q) {
//        cout << "Iteration " << q << endl;
        std::vector<int> pts1 = Util::pickRandVals(good1, NUM_PTS),
            pts2 = Util::pickRandVals(good2, NUM_PTS);

        std::vector<Matrix> pass1;
        std::vector<Matrix> pass2;
        Matrix temp(2,1);
        for (int i=0; i<(int)pts1.size(); ++i) {
            temp.set(0,0,fl1->feature[pts1[i]]->x);
            temp.set(1,0,fl1->feature[pts1[i]]->y);
            pass1.push_back(temp);
        }
        for (int i=0; i<(int)pts2.size(); ++i) {
            temp.set(0,0,fl2->feature[pts2[i]]->x);
            temp.set(1,0,fl2->feature[pts2[i]]->y);
            pass2.push_back(temp);
        }
        EpiGeom thisPass(pass1, pass2);
        double thisScore = scoreF3(thisPass.getFund(), fl1, fl2);
        if (q == 0 || thisScore > bestScore) {
            retVal = thisPass;
            bestScore = thisScore;
        }
    }

    KLTFreeFeatureList(fl1);
    KLTFreeFeatureList(fl2);
    return retVal;
}

EpiGeom getFundamentalMatrix(VBPtr img1, VBPtr img2) {
    int numPts = Globals::NUM_PTS;
    KLT_FeatureList fl1 = getFeatures(img1, Globals::NUM_FEATS);
    KLT_FeatureList fl2 = getFeatures(img2, Globals::NUM_FEATS);
    std::vector<int> good1 = goodIndices(fl1),
        good2 = goodIndices(fl2);
    numPts = Util::min<int>(numPts, (int)good1.size());
    numPts = Util::min<int>(numPts, (int)good2.size());

    EpiGeom retVal;
    Matrix testVal(3,3);
    testVal.set(2,2,1);

    Matrix x(8,1);
    Matrix b(numPts, 1);
    for (int i=0; i<numPts; ++i)
        b.set(i,0,-1);
    Matrix a(numPts,8);
    double bestScore = 0;

    for (int q=0; q<Globals::NUM_ITER; ++q) {
        std::vector<int> pts1 = Util::pickRandVals(good1, numPts),
            pts2 = Util::pickRandVals(good2, numPts);

        std::vector<Matrix> pass1;
        std::vector<Matrix> pass2;
        Matrix temp(2,1);
        for (int i=0; i<(int)pts1.size(); ++i) {
            temp.set(0,0,fl1->feature[pts1[i]]->x);
            temp.set(1,0,fl1->feature[pts1[i]]->y);
            pass1.push_back(temp);
        }
        for (int i=0; i<(int)pts2.size(); ++i) {
            temp.set(0,0,fl2->feature[pts2[i]]->x);
            temp.set(1,0,fl2->feature[pts2[i]]->y);
            pass2.push_back(temp);
        }
        EpiGeom thisPass(pass1, pass2);
        double thisScore = scoreF(thisPass.getFund(), fl1, fl2);
        if (q == 0 || thisScore > bestScore) {
            retVal = thisPass;
            bestScore = thisScore;
        }
    }

    KLTFreeFeatureList(fl1);
    KLTFreeFeatureList(fl2);
    return retVal;
}
