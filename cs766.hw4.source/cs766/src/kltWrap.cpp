#include "kltWrap.h"
#include "globals.h"
#include <ipl.h>
#include <cv.h>

//static const char* DEFAULT_FEATURE_NUM = "1000";
static const int MIN_FEATURES = 50;
static const int INC_FEATURES_BY = 500;

/**
 * Make a copy of this feature list
 */
KLT_FeatureList KLTCloneFeatureList(KLT_FeatureList fl) {
    KLT_FeatureList retVal = KLTCreateFeatureList(fl->nFeatures);
    for (int i=0; i<fl->nFeatures; ++i) {
        *(retVal->feature[i]) = *(fl->feature[i]);
    }
    return retVal;
}

/**
 * Run KLT on two images
 */
std::pair<KLT_FeatureList, KLT_FeatureList> getFeatures(VBPtr one, VBPtr two, int numFeatures)
{
    using namespace std;
    using namespace ImageMan;
    pair<KLT_FeatureList, KLT_FeatureList> retVal = make_pair(KLT_FeatureList(0), KLT_FeatureList(0));

    VBPtr gray1(new ValueBlock(one->getNumChannels(), one->getRegion(), one->getDepth()));
    VBPtr gray2(new ValueBlock(two->getNumChannels(), two->getRegion(), two->getDepth()));
    cvCopyImage(one.get(), gray1.get());
    cvCopyImage(two.get(), gray2.get());

    gray1->convert(1, IT_UNSIGNED_CHAR);
    gray2->convert(1, IT_UNSIGNED_CHAR);
    int h1 = one->getHeight();
    int h2 = two->getHeight();
    int w1 = one->getWidth();
    int w2 = two->getHeight();
    unsigned char *imgOne = new unsigned char[w1*h1];
    unsigned char *imgTwo = new unsigned char[w2*h2];
    // Make sure there's no row padding
    for (int i=0; i<h1; ++i)
        memcpy(imgOne+i*w1, gray1->getRow(i), w1);
    for (int i=0; i<h2; ++i)
        memcpy(imgTwo+i*w2, gray2->getRow(i), w2);

    KLT_TrackingContext tc = KLTCreateTrackingContext();
        tc->window_height=15;
        tc->window_width=15;
        tc->max_iterations=1000;
KLTUpdateTCBorder(tc);
    tc->sequentialMode = TRUE;
    tc->writeInternalImages = FALSE;
    tc->affineConsistencyCheck = -1;  /* set this to 2 to turn on affine consistency check */

    KLT_FeatureList fl, fl2 = KLTCreateFeatureList(numFeatures);
    KLTSelectGoodFeatures(tc, imgOne, w1, h1, fl2);
    fl = KLTCloneFeatureList(fl2);
    retVal.first = fl;
        
    KLTTrackFeatures(tc, imgOne, imgTwo, w2, h2, fl2);
    retVal.second = fl2;

    KLTFreeTrackingContext(tc);
    delete []imgOne;
    delete []imgTwo;
    return retVal;
}

KLT_FeatureList getFeatures(VBPtr one, int numFeatures) {
    using namespace std;
    using namespace ImageMan;

    VBPtr gray1(new ValueBlock(one->getNumChannels(), one->getRegion(), one->getDepth()));
    cvCopyImage(one.get(), gray1.get());

    gray1->convert(1, IT_UNSIGNED_CHAR);
    int h1 = one->getHeight();
    int w1 = one->getWidth();
    unsigned char *imgOne = new unsigned char[w1*h1];
    // Make sure there's no row padding
    for (int i=0; i<h1; ++i)
        memcpy(imgOne+i*w1, gray1->getRow(i), w1);
    KLT_TrackingContext tc = KLTCreateTrackingContext();
/*        tc->window_height=101;
    tc->window_width=51;*/
    tc->sequentialMode = TRUE;
    tc->writeInternalImages = FALSE;
    tc->affineConsistencyCheck = -1;  /* set this to 2 to turn on affine consistency check */

    KLT_FeatureList fl = KLTCreateFeatureList(numFeatures);
    KLTSelectGoodFeatures(tc, imgOne, w1, h1, fl);
    KLTFreeTrackingContext(tc);
    delete []imgOne;
    return fl;
}

std::vector<int> goodIndices(KLT_FeatureList fl) {
    std::vector<int> goodPts;
    for (int i=0; i<fl->nFeatures; ++i)
        if (fl->feature[i]->val != KLT_NOT_FOUND)
            goodPts.push_back(i);
    return goodPts;
}

std::vector<int> goodIndices(const std::pair<KLT_FeatureList, KLT_FeatureList>& tracked) {
    std::vector<int> goodPts;
    for (int i=0; i<tracked.first->nFeatures; ++i)
        if (tracked.first->feature[i]->val != KLT_NOT_FOUND && 
            tracked.second->feature[i]->val == KLT_TRACKED)
            goodPts.push_back(i);
    return goodPts;
}
