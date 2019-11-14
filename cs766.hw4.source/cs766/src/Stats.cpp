#include "Stats.h"
#include <cstdlib>
#include <ctime>
#include "debug.h"
#include <cmath>

using namespace std;
using namespace ImageMan;

Stats Stats::init;
const int Stats::_largeNumDbl = 4;
const int Stats::_largeNumInt = 3;
std::vector<bool> Stats::_largeIsDbl;
std::vector<double> Stats::_largeHelpDbl;
std::vector<LargeInt> Stats::_largeHelpInt;

int Stats::round(double val) {
    if ((val-(int)val) >= .5)
        return (int)val+1;
    else return (int)val;
}

Stats::Stats() {
    // Set rand seed
    srand( (unsigned)time( NULL ) );
}

double Stats::randVal(double minVal, double maxVal) {
    return minVal+(maxVal-minVal)*(double)rand()/RAND_MAX;
}

Region Stats::randRegion(const Region& src, int width, int height) {
    double centX = randVal(src.left(), src.right());
    double centY = randVal(src.top(), src.bottom());
    centX -= width/2.0;
    centY -= height/2.0;
    Region randReg(round(centX), round(centY), width, height);
    return randReg.intersectWith(src);
}

Region Stats::randRegion(const Region& src) {
    Region retVal;
    do {
        retVal = randRegion(src, (int)randVal(0, src.width()), (int)randVal(0, src.height()));
    } while (retVal.area()==0);
    return retVal;
}

double Stats::fractionFilled(VBHandle img, const Region& reg) {
    ValueBlockIterator i = img->getIterator(reg);
    int num = 0;
    int numFilled = 0;
    while (!i.passedRegionEnd()) {
        ++num;
        if (i.getUChar())
            ++numFilled;
        i.inc();
    }
    return (double)num/(double)numFilled;
}

double Stats::fractionFilled(const vector<VBHandle>& stack, const Region& reg) {
    vector<VBHandle>::const_iterator i;
    int size = (int)stack.size();
    double retVal = 0;
    for (i = stack.begin(); i != stack.end(); ++i)
        retVal += fractionFilled(*i, reg)/size;
    return retVal;    
}

double Stats::average(const vector<double>& vals) {
    double sum = 0;
    vector<double>::const_iterator i;
    for (i = vals.begin(); i != vals.end(); ++i)
        sum += *i;
    return sum/vals.size();
}

double Stats::stdDev(const vector<double>& vals) {
    double ave = average(vals);
    double retVal = 0;
    vector<double>::const_iterator i;
    for (i = vals.begin(); i != vals.end(); ++i)
        retVal += pow(*i-ave,2);
    retVal /= vals.size()-1;
    return sqrt(retVal);
}

double Stats::max(const std::vector<double>& vals) {
    double max = vals[0];
    vector<double>::const_iterator i;
    for (i = vals.begin()+1; i != vals.end(); ++i)
        if (*i > max) max = *i;
    return max;
}

double Stats::min(const std::vector<double>& vals) {
    double min = vals[0];
    vector<double>::const_iterator i;
    for (i = vals.begin()+1; i != vals.end(); ++i)
        if (*i < min) min = *i;
    return min;
}

void Stats::largeInit(int set) {
    if ((int)_largeHelpDbl.size() < _largeNumDbl*(set+1))
        _largeHelpDbl.resize(_largeNumDbl*(set+1));
    if ((int)_largeHelpInt.size() < _largeNumInt*(set+1))
        _largeHelpInt.resize(_largeNumInt*(set+1));
    if ((int)_largeIsDbl.size() < set+1)
        _largeIsDbl.resize(set+1);
    for (int i=0; i<_largeNumDbl; ++i)
        dval(set, i) = 0;
    for (int i=0; i<_largeNumInt; ++i)
        ival(set, i) = 0;
    _largeIsDbl[set] = false;
}

void Stats::largeAdd(double val, int set) {
    // Do max/min
    if (ival(set, 0) == LargeInt(0)) {
        dval(set, 2) = dval(set, 3) = val;
    } else {
        if (val > dval(set, 2))
            dval(set, 2) = val;
        if (val < dval(set, 3))
            dval(set, 3) = val;
    }
    // Convert to double calcs if haven't seen a double yet
    if (!_largeIsDbl[set]) {
        _largeIsDbl[set] = true;
        dval(set,0) = ival(set,1);
        dval(set,1) = ival(set,2);
    }
    // Break up std dev into its pieces, and add in the
    // average contribution at the end
    ival(set, 0) += 1;
    dval(set, 0) += val;
    dval(set, 1) += val*val;
}

void Stats::largeAdd(int val, int set) {
    // If we've seen a double, move to the double calc
    if (_largeIsDbl[set])
        largeAdd((double)val, set);

    // Do max/min
    if (ival(set, 0) == LargeInt(0)) {
        dval(set, 2) = dval(set, 3) = val;
    } else {
        if (val > dval(set, 2))
            dval(set, 2) = val;
        if (val < dval(set, 3))
            dval(set, 3) = val;
    }

    // Break up std dev into its pieces, and add in the
    // average contribution at the end
    ival(set, 0) += 1;
    ival(set, 1) += val;
    ival(set, 2) += val*val;
}

double Stats::largeAve(int set) {
    if (_largeIsDbl[set])
        return dval(set, 0)/ival(set, 0);
    else return (double)ival(set,1)/ival(set,2);
}

double Stats::largeStdDev(int set) {
    double ave = largeAve();
    double numer;
    if (_largeIsDbl[set])
        numer = dval(set,1)-2*dval(set,0)*ave+ave*ave*ival(set,0);
    else
        numer = (double)ival(set,2)-2*ival(set,1)*ave+ave*ave*ival(set,0);
    return sqrt(numer/(ival(set,0)-LargeInt(1)));
}

double Stats::largeMax(int set) {
    return dval(set, 2);
}

double Stats::largeMin(int set) {
    return dval(set, 3);
}
