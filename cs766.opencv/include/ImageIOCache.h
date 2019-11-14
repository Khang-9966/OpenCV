#pragma once

#include "ImageIO.h"
#include <map>
#include <list>
#include "imdllexp.h"

namespace ImageMan {

    /**
     * IO Store that acts like a cache.  Note that this does not factory images
     */
    class IM_DLL ImageIOCache : public ImageIO {
    public:
        ImageIOCache();
        virtual ~ImageIOCache();

    public:
        void setCacheImages(unsigned int num);
    private:
        void setCacheSize(unsigned int newSize);

    public: // ImageIO overrides
        /**
        * Load an image corresponding to a baseID and a viewID.
        *
        * @param baseID ID Corresponding to an image set
        * @param viewID ID Corresponding the a particular view in the image set
        * @param type Not used.  This class does not factory images
        * @return null if Image does not exist in the store
        */
        virtual void loadImage(VBHandle& handle);

        /**
        * Save an image corresponding to a baseID and a viewID.
        *
        * @param baseID ID Corresponding to an image set
        * @param viewID ID Corresponding the a particular view in the image set
        * @param view View to store
        */
        virtual void saveImage(ImageMan::VBHandle handle);

        /**
        * Remove an image from the store.
        *
        * @param baseID ID Corresponding to an image set
        * @param viewID ID Corresponding the a particular view in the image set
        */
        virtual void destroyImage(const char* baseID,
            const char* viewID);

        void saveCache(ImageIO* save);
        void damageCache();
        void updateCache();
        void clearCache();
        virtual bool containsImage(const char* baseID, const char* viewID);
    private:
        void evict(const std::string& id);
        void reference(const std::string& id);
        int cacheSize(VBHandle image);
        std::string makeId(const std::string& baseID, const std::string&viewID) const;

    private:
#pragma warning(push)
#pragma warning(disable:4251)
        unsigned int m_numImages;
        unsigned int m_largestSeen;
        unsigned int m_curCache;
        unsigned int m_maxCache;
        std::map<std::string, VBHandle> m_idToCacheable;
        std::list<std::string> m_idAccessList;
#pragma warning(pop)
    };

};

