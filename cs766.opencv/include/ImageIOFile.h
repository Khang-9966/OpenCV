#pragma once

#include "ImageIOCache.h"
#include "imdllexp.h"

namespace ImageMan {

    class IM_DLL ImageIOFile : public ImageIOCache {
    public:
        ImageIOFile();
        virtual ~ImageIOFile();

    public:
        inline void setWorkingDir(const char* dir) {
            m_workingDir = dir;
        }

        inline const char* const getWorkingDir() const {
            return m_workingDir.c_str();
        }

    public: // ImageIO overrides
        /**
        * Load an image corresponding to a baseID and a viewID.
        *
        * @param baseID ID Corresponding to an image set
        * @param viewID ID Corresponding the a particular view in the image set
        * @return null if Image does not exist in the store
        */
        virtual void loadImage(VBHandle& handle);;

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

        virtual bool containsImage(const char* baseID, const char* viewID);
    public:
        /**
         * Determine if a file exists
         * 
         * @return true if file exists, false otherwise
         */
        static bool fileExists(const std::string &filename);

    private:

        /**
        * Constructs a filename from the working dir, baseID, and viewID
        */
        std::string constructFilename(const std::string &baseID,
            const std::string &viewID);
    private:
#pragma warning(push)
#pragma warning(disable: 4251)
        std::string m_workingDir;
#pragma warning(pop)
    };

};
