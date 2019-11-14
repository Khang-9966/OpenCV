#ifndef KLTWRAP_H
#define KLTWRAP_H

#include <klt.h>
#include "ArgManager.h"
#include <vector>
#include "VBPtr.h"

KLT_FeatureList KLTCloneFeatureList(KLT_FeatureList fl);
std::pair<KLT_FeatureList, KLT_FeatureList> getFeatures(VBPtr one, VBPtr two, int num);
KLT_FeatureList getFeatures(VBPtr one, int num);
std::vector<int> goodIndices(KLT_FeatureList fl);
std::vector<int> goodIndices(const std::pair<KLT_FeatureList, KLT_FeatureList>& tracked);
#endif