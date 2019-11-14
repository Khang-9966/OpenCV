#pragma once
#include "imdllexp.h"
#include "ImageUtil.h"
#include "Texture.h"
#include "debug.h"
#include <ITImage.h>
namespace ImageMan {
    class ImageIO;
    class ImageFilter;

    /**
     * A class that makes the iteration over ValueBlocks more convenient.
     *
     * These iterators can be requested over any region.  A get or set off the
     * region will result in zero and noop respectively.  Type casts are done
     * automatically, even if data might be lost (e.g. set called on an int
     * block passing a double as a parameter).
     */
    class IM_DLL ValueBlockIterator {
        /**
         * A nested class which takes advantage of the vft to direct method calls.
         *
         * This class exists to use a vft instead of a switch statement everywhere.
         * It seems to be a little bit faster that way.
         */
        class VBIHelper {
        public:
            VBIHelper(ITImage* img,
                const Region& srcRegion, const Region& iterRegion,
                int x, int y);

            void setPos(int x, int y);
            virtual double getDbl(int channel = 0) const = 0;
            virtual float getFloat(int channel = 0) const = 0;
            virtual int getInt(int channel = 0) const = 0;
            virtual unsigned int getUInt(int channel = 0) const = 0;
            virtual char getChar(int channel = 0) const = 0;
            virtual unsigned char getUChar(int channel = 0) const = 0;

            virtual void set(double val, int channel = 0) const = 0;
            virtual void set(float val, int channel = 0) const = 0;
            virtual void set(int val, int channel = 0) const = 0;
            virtual void set(char val, int channel = 0) const = 0;
            virtual void set(unsigned int val, int channel = 0) const = 0;
            virtual void set(unsigned char val, int channel = 0) const = 0;
            virtual void inc();
            virtual void dec();
            virtual void setIterRegion(const Region& region);
            
            /**
             * Clone the iterator
             */
            virtual VBIHelper* clone()=0;

            /**
             * Get the x coordinate of the iterator
             */
            inline int getX() const {
                return m_x; }

            /**
             * Get the y coordinate of the iterator
             */
            inline int getY() const {
                return m_y; }

            /**
             * After an inc, returns true if we've stepped off the end of the iterator region
             */
            inline bool passedRegionEnd() const {
                return m_bRegionEnd; }

            /**
             * After an dec, returns true if we've stepped off the end of the iterator region
             */
            inline bool passedRegionStart() const {
                return m_bRegionStart; }

            /**
             * Retrieve the region we're iterating over
             */
            const Region& getIteratorRegion() const {
                return m_iterRegion; }

            /**
             * Retrieve the region that corresponds to the ValueBlock we're
             * iterating over
             */
            const Region& getSourceRegion() const {
                return m_srcRegion; }

            /**
             * Determine the number of channels in this image
             */
            inline int numChannels() const {
                return m_img->getNumChannels(); }

            /**
             * Determine if this iterator is pointing to a valid location in the block
             */
            inline bool isValid() const {
                return m_srcRegion.pointIn(m_x, m_y);
            }

            inline int getType() const {
                return m_img->getDepth(); }
            
            virtual void* getPtr(int channel=0) const;

        protected:
            ITImage *m_img;
            /** The x coordinate of the iterator */
            int m_x,
            /** The y coordinate of the iterator */
                m_y;
            /** Pointer to the start of the value block */
            void *m_start,
            /** Pointer to the current position */
                *m_pos;
            /** True after an inc if we passed the end of the iterating region */
            bool m_bRegionEnd,
            /** True after a dec if we passed the end of the iterating region */
                m_bRegionStart;
            /** Region corresponding to the ValueBlock */
            Region m_srcRegion;
            /** Region corresponding to where we're iterating */
            Region m_iterRegion;
            /** Precalculated constant for incrementing X at the end of a row */
            int m_offX,
            /** Precalculated constant for incrementing the pointer at the end of a row */
                m_offPos;
        };
        class VBIHelperDbl;
        class VBIHelperFloat;
        class VBIHelperInt;
        class VBIHelperUInt;
        class VBIHelperChar;
        class VBIHelperUChar;
    public:
        ValueBlockIterator(ITImage* img,
            const Region& srcRegion, const Region& iterRegion,
            int x, int y);

        ~ValueBlockIterator();
        ValueBlockIterator(const ValueBlockIterator& rhs);
#ifndef SWIG
        ValueBlockIterator& operator=(const ValueBlockIterator& rhs);
#endif

        inline double getDbl(int channel = 0) const {
            return m_helper->getDbl(channel); }

        inline float getFloat(int channel = 0) const {
            return m_helper->getFloat(channel); }

        inline int getInt(int channel = 0) const {
            return m_helper->getInt(channel); }

        inline unsigned int getUInt(int channel = 0) const {
            return m_helper->getUInt(channel); }

        inline char getChar(int channel = 0) const {
            return m_helper->getChar(channel); }

        inline unsigned char getUChar(int channel = 0) const {
            return m_helper->getUChar(channel); }

        inline void set(double val, int channel = 0) const {
            m_helper->set(val, channel); }

        inline void set(float val, int channel = 0) const {
            m_helper->set(val, channel); }

        inline void set(int val, int channel = 0) const {
            m_helper->set(val, channel); }

        inline void set(char val, int channel = 0) const {
            m_helper->set(val, channel); }

#ifndef SWIG
        inline void set(unsigned int val, int channel = 0) const {
            m_helper->set(val, channel); }

        inline void set(unsigned char val, int channel = 0) const {
            m_helper->set(val, channel); }
#endif
        inline int getX() {
            return m_helper->getX(); }

        inline int getY() {
            return m_helper->getY(); }

        inline void inc() {
            return m_helper->inc(); }

        inline void dec() {
            return m_helper->dec(); }

        inline bool passedRegionEnd() {
            return m_helper->passedRegionEnd(); }

        inline bool passedRegionStart() {
            return m_helper->passedRegionStart(); }

        inline int getType() const {
            return m_helper->getType(); }

        inline const Region& getIteratorRegion() {
            return m_helper->getIteratorRegion(); }

        inline const Region& getSourceRegion() const {
            return m_helper->getSourceRegion(); }

        inline int numChannels() const {
            return m_helper->numChannels(); }

        inline bool isValid() const {
            return m_helper->isValid(); }

//        inline void* getPtr(int channel=0) const {
//            return m_helper->getPtr(channel); }

        inline void setIterRegion(const Region& region) const {
            m_helper->setIterRegion(region); }

    private:
        VBIHelper* m_helper;
    };

    /**
     * A generic typed block which can be accessed without consideration of type
     *
     * !!!WARNING: Do not add any virtual methods to this object.  For some reason,
     * VC does not do the implicit cast correctly when calling into OpenCV if there
     * is a vft on this instance.
     */
#pragma warning(push)
#pragma warning(disable:4275)
    class IM_DLL ValueBlock : public ITImage {
#pragma warning(pop)
    public:
        ValueBlock(ITImage* img);
        /**
         * Create a block of type large enough for region and channels.
         */
        ValueBlock(int channels, const Region& region, int type);

        ~ValueBlock();

        ValueBlock* clone();

        /**
         * Retrieve an iterator that will iterate over the whole block, starting at upper-left
         */
        ValueBlockIterator getIterator();

        /**
         * Retrieve an iterator that will iterate over the whole block, starting at x, y
         */
        ValueBlockIterator getIterator(int x, int y);

        /**
         * Retrieve an iterator that will iterate over the given region, starting at upper-left
         */
        ValueBlockIterator getIterator(const Region& region);

        /**
         * Retrieve an iterator that will iterate over the give region, starting at x, y
         */
        ValueBlockIterator getIterator(int x, int y, const Region& region);

        /**
         * Retrieve a region corresponding to the dimensions of the block
         */
        inline const Region& getRegion() const {
            return m_reg; }

        /**
         * Retrieve the number of channels in the block
         */
        int getNumChannels() const {
            return ITImage::getNumChannels(); }

        /**
         * Get the width of the block
         */
        int getWidth() const {
            return m_reg.width(); }

        /**
         * Get the height of the block
         */
        int getHeight() const {
            return m_reg.height(); }

        /**
         * Set all values in the block to 0
         */
        inline void zero() {
            setAll(0);
        }

        /**
         * Set all values in the given region to 0
         */
        inline void zero(const Region& reg) {
            setAll(reg, 0);
        }
        
        /**
         * Set all values in the block to val
         */
        void setAll(unsigned char val);
        
        /**
         * Set all values in the given region to val
         */
        void setAll(const Region& reg, unsigned char val);

    private: // Use clone
        /** Copy CTOR - Do not use */
        ValueBlock(const ValueBlock& rhs);
        /** Assignment - Do not use */
        ValueBlock& operator=(const ValueBlock& rhs);

    protected:
        Region m_reg;
    };

    /**
     * Value Block management, loader, and reference counter
     *
     * This class is more than just a smart pointer.  It not only maintains the
     * number of references to the ValueBlock, but also has intelligence built in
     * to know how to generate the ValueBlock when it needs to be loaded.
     */
    class IM_DLL VBHandle : public Damageable {
        struct Info;
    public:
        /**
         * Loading policy
         */
        enum eLoadPolicy {
            /** Check the cache first, then load from disk */
            L_CaLo,
            /** Check the cache first, then load from disk, then try to use filter to generate it */
            L_CaLoFi,
            /** Check the cache first, then try to use filter to generate it */
            L_CaFi 
        };

        /**
         *Where to load from when loading from something other than cache or filter
         */
        enum eLoadSource {
            L_File,     /** Load from disk */
            L_Database  /** Load from database connection */
        };

        /**
         * Store policy when ValueBlocks are deleted
         */
        enum eStorePolicy {
            /** Save before delete */
            S_Save,
            /** Save an image with a non-zero entry */
            S_SaveNonEmpty, 
            /** Just delete it */
            S_Delete
        };

        /**
         * Synchronizatio Policy
         */
        enum eSynchPolicy {
            /** Keep synchronized through filters */
            S_Synch,  
            /** Do not use filter after creation after initial load */
            S_Unsynch 
        };

        /** Create an empty handle (null-pointer) */
        VBHandle();

        /** Construct a no frills value block handle */
        VBHandle(ValueBlock* block);

        /**
         * Construct a smart handle
         * 
         * @param ioCache ImageIO object to use when testing if something is in the cache
         * @param ioStore ImageIO object to use for loading from storage
         * @param load Load policy to assign to this handle
         * @param store Store policy to assign to this handle
         * @param synch Synchronization policy to assign to this handle
         * @param baseID BaseID to load
         * @param viewID ViewID to load
         * @param src Source filter if there is one, null otherwise
         */
        VBHandle(ImageIO *ioCache,
            ImageIO *ioStore,
            eLoadPolicy load,
            eLoadSource loadSrc,
            eStorePolicy store,
            eSynchPolicy synch,
            const char* baseID,
            const char* viewID,
            ImageFilter* src=0);

        /**
         * Convenience constructor to just load something from storage
         */
        VBHandle(ImageIO *ioStore, const char* filename);

        virtual ~VBHandle();

        /**
         * Generate a copy of this handle, without an allocated block
         */
        VBHandle emptyHandle() const;
    public:
        /**
         * Load this from cache/disk/filter/whatever.
         *
         * Note that these are reference counted as well.  Each reference is allowed
         * one load, which calls unload in the DTOR.
         */
        void load();      

        /**
         * Unload/delete this thing.
         *
         * Note that these are reference counted as well
         */
        void unload();    

        /**
         * Determine if the ValueBlock is loaded
         */
        inline bool isLoaded() const {
            return m_bLoad; }

        /**
         * Write to disk without unloading
         */
        void save();      

        /**
         * Retrieve the base id of the handle
         */
        const char* getBaseID() const;

        /**
         * Retrieve the view id of the handle
         */
        const char* getViewID() const;

        /**
         * Set the base id of the handle
         */
        void setViewID(const char* viewID);

        /**
         * Set the base id of the handle
         */
        void setBaseID(const char* baseID);

        /**
         * Damage a particular region of the value block (just to override superclass)
         *
         * Call damage(region) instead!
         */
        virtual void damage(const char* baseID, const Region &region);

        /**
         * Damage a particular region of the value block
         */
        virtual void damage(const Region& region);

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
         * Update the ValueBlock's damaged region based on source filters
         */
        virtual void updateBlock();

        /**
         * Update the texture representation of this ValueBlock if one exists
         */
        virtual void updateTexture();

        virtual int getWidth();

        virtual int getHeight();

        virtual int numChannels();

        /**
         * Set the ValueBlock that this handle points to - probably shouldn't call this
         */
        void setImg(ValueBlock* vb);

    public: // Don't call these
        /**
         * Called by addDepedent - Don't call this!
         */
        virtual void addSource(Damageable* source);

        /**
         * Called by removeDepedent - Don't call this!
         */
        virtual void removeSource(Damageable* source);

    public: // Smart pointer stuff
        int loadCount();
        /**
         * Copy CTOR
         */
        VBHandle(const VBHandle& r);
#ifndef SWIG
        /**
         * Is this pointer valid?
         */
        operator bool() const;
        
        /**
         * Assignment op
         */
        VBHandle& operator=(const VBHandle& r);
#endif
        /**
         * Dereference the pointer
         */
        ValueBlock& operator*();

        /**
         * Call on the internal pointer
         */
        ValueBlock* operator->();

        ValueBlock* get();

        /**
         * Get a pointer to a texture if one exists
         */
        TexturePtr getTex();

        /**
         * Is this reference unique?
         */
        bool unique() const;

    private:
        void acquire(Info* c);
        void release();

    private:
        Info *m_info;
        mutable bool m_bLoad;

    public:
        static int _debug_refs;
        static int _debug_loads;
    };
};
