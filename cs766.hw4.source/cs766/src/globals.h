#ifndef GLOBALS_H
#define GLOBALS_H
#include "ArgManager.h"

class Globals {
public:
    static ArgManager* gArgManager;
    static int NUM_FEATS;
    static int NUM_ITER;
    static int NUM_PTS;
    static int PILU_WIN_SIZE;
    static double PILU_SIGMA;
};
#endif