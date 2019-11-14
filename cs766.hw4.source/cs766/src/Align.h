#ifndef ALIGN_H
#define ALIGN_H

#include "VBPtr.h"
#include "LargeInt.h"
#include <Matrix.h>
#include "EpiGeom.h"

/**
 * Get the median translation as determined by tracked features
 */
ImageMan::Matrix getMedianTranslation(VBPtr from, VBPtr to);

/**
 * Get the average translation determined by tracked features
 */
ImageMan::Matrix getAverageTranslation(VBPtr from, VBPtr to);

LargeInt score(const ImageMan::Matrix& transform, VBPtr from, VBPtr to);

EpiGeom getFundamentalMatrix(VBPtr img1, VBPtr img2);
EpiGeom getFundamentalMatrix2(VBPtr img1, VBPtr img2);
EpiGeom getFundamentalMatrix3(VBPtr img1, VBPtr img2);
EpiGeom getFundamentalMatrix4(VBPtr img1, VBPtr img2);


#endif
