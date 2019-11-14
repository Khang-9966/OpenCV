#pragma once
#include "imdllexp.h"
#include <list>
#include <ipl.h>

namespace ImageMan {
    /**
     * Representation of a 2D region
     */
#ifndef SWIG
    class IM_DLL Region : public _IplROI {
#else
    class IM_DLL Region {
#endif
    public:
        /**
         * Create an invalid/empty region
         */
        Region();

        /**
         * Create a region with upper-left at x, y, and has width/height
         */
        Region(int x, int y, int width, int height);

        /**
         * Create a region from an IPL ROI
         */
        Region(const _IplROI& iplReg);

        /**
         * Find the overlapping region
         */
        Region intersectWith(const Region& rhs) const;

        /**
         * Find the region that contains both regions
         */
        Region unionWith(const Region& rhs) const;

        /**
         * Determine the area of the region
         */
        inline int area() const {
            return width()*height();
        }

        /**
         * Determine if a point is in the region
         */
        inline bool pointIn(int x, int y) const {
            return x >= xOffset && x < (xOffset+_IplROI::width) &&
                y >= yOffset && y < (yOffset+_IplROI::height);
        }

        /**
         * Determine the width of the region
         */
        inline int width() const {
            if (bValid)
                return _IplROI::width;
            else return 0;
        }
        
        /**
         * Determine the height of the region
         */
        inline int height() const {
            if (bValid)
                return _IplROI::height;
            else return 0;
        }
    
        /**
         * Determine the top of the region 
         */
        inline int top() const {
            return _IplROI::yOffset;
        }

        /**
         * Determine the bottom of the region 
         */
        inline int bottom() const {
            return _IplROI::yOffset+_IplROI::height-1;
        }

        /**
         * Determine the left of the region 
         */
        inline int left() const {
            return _IplROI::xOffset;
        }

        /**
         * Determine the right of the region 
         */
        inline int right() const {
            return _IplROI::xOffset+_IplROI::width-1;
        }

        inline void adjustBy(int left, int right, int top, int bottom) {
            _IplROI::height += -top+bottom;
            _IplROI::width += -left+right;
            xOffset += left;
            yOffset += top;
        }

        /**
         * Determine if two regions are equivalent
         */
        bool operator==(const Region& rhs);

        /**
         * Determine if two regions are not-equivalent
         */
        inline bool operator!=(const Region& rhs) {
            return !(*this == rhs); }

        bool bValid;
    };

    /**
     * Superclass for managing damage of regions
     */
    struct IM_DLL Damageable {
        virtual ~Damageable();
        /**
         * Damage a particular region of the value block
         */
        virtual void damage(const char* baseID, const Region& region);

        /**
         * Add a dependent image.
         *
         * When this image is damaged, dependent images are also damaged.
         */
        virtual void addDependent(Damageable* dependent);

        /**
         * Remove a dependent image.
         *
         * When this image is damaged, dependent images are no longer damaged.
         */
        virtual void removeDependent(Damageable* dependent);

        /**
         * Initialize for a particular baseID
         */
        virtual bool init(const char* baseID);

        /** Get the width of the object */
        virtual int getWidth()=0;

        /** Get the height of the object */
        virtual int getHeight()=0;

        /** Get the number of channels of the object */
        virtual int numChannels()=0;
    public: //Don't call these
        /**
         * Called by addDepedent - Don't call this!
         */
        virtual void addSource(Damageable* source);

        /**
         * Called by removeDepedent - Don't call this!
         */
        virtual void removeSource(Damageable* source);
#pragma warning(push)
#pragma warning(disable:4251)
        /** List of dependents */
        std::list<Damageable*> m_dependents;
        /** List of sources */
        std::list<Damageable*> m_sources;
#pragma warning(pop)
    public:
        /** Region that has been damaged */
        Region m_dam;
    };
};
