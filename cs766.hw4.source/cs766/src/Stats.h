#pragma once

#include <vector>
#include <ValueBlock.h>
#include <ImageUtil.h>
#include "LargeInt.h"

/**
 * A collection of simple stats/math info
 */
class Stats {
private: // Static methods only
    /** Initialize static info and random number gen */
    Stats();
public:
    /** Round the value */
    static int round(double val);

    /**
     * Find a random region in the src region with a given width and height.
     * 
     * The region is trimmed to be inside the src region
     */
    static ImageMan::Region randRegion(const ImageMan::Region& src,
        int width, int height);

    /**
     * Find a random region with a random width and random height
     * inside of the src region
     */
    static ImageMan::Region randRegion(const ImageMan::Region& src);

    /**
     * Get a random value [minVal..maxVal]
     */
    static double randVal(double minVal, double maxVal);

    /**
     * Determine the fraction of the region of the img that is filled
     */
    static double fractionFilled(ImageMan::VBHandle img, 
        const ImageMan::Region& reg);

    /**
     * Determine the fraction of the volume that is filled
     *
     * Assumes all slices in the stack are equal depth
     */
    static double fractionFilled(
        const std::vector<ImageMan::VBHandle>& stack,
        const ImageMan::Region& reg);

    /** Find the average of a list of numbers */
    static double average(const std::vector<double>& vals);
    /** Find the std dev of a list of numbers - using n-1 version */
    static double stdDev(const std::vector<double>& vals);
    /** Find the max value of a list of numbers */
    static double max(const std::vector<double>& vals);
    /** Find the min value of a list of numbers */
    static double min(const std::vector<double>& vals);

    /**
     * Initialize a stats counting set for a large number of items to
     * count.
     */
    static void largeInit(int set = 0);

    /** Add a value to an initialized set */
    static void largeAdd(double dbl, int set = 0);
    /** Add a value to an initialized set */
    static void largeAdd(int val, int set = 0);

    /** Determine the average of a large counting set */
    static double largeAve(int set = 0);

    /** Determine the std dev of a large counting set */
    static double largeStdDev(int set = 0);

    /** Determine the max value in the large counting set */
    static double largeMax(int set = 0);

    /** Determine the min value in the large counting set */
    static double largeMin(int set = 0);

private: // Helpers for looking up indices, values
    inline static int iidx(int set, int which) {
        return _largeNumInt*set+which; }
    inline static int didx(int set, int which) {
        return _largeNumDbl*set+which; }
    inline static LargeInt& ival(int set, int which) {
        return _largeHelpInt[iidx(set, which)]; }
    inline static double& dval(int set, int which) {
        return _largeHelpDbl[didx(set, which)]; }

    static const int _largeNumDbl;
    static const int _largeNumInt;

#pragma warning(push)
#pragma warning (disable:4251)
    static std::vector<bool> _largeIsDbl;
    static std::vector<double> _largeHelpDbl;
    static std::vector<LargeInt> _largeHelpInt;
    // dval 0 - sum(val)
    // dval 1 - sum(val*val)
    // dval 2 - max
    // dval 3 - min
    // ival 0 - count
    // ival 1 - sum(val)
    // ival 2 - sum(val*val)
#pragma warning(pop)
    // Get the init code to call
    static Stats init;
};
