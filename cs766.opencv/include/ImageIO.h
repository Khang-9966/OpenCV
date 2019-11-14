#pragma once
#include <string>
#include "imdllexp.h"

namespace ImageMan {
    class VBHandle;

    /**
    * Interface for loading and storing image views.
    *
    * Subclasses may do things like load/store images in a filesystem or
    * perhaps a database.  The semantics of baseID's and viewID's depends
    * on the subclass.
    */
    class IM_DLL ImageIO {
    public:
        ImageIO() {}
        virtual ~ImageIO() {}

    public:
        /**
         * Load an image corresponding to a baseID and a viewID.
         *
         * @param baseID ID Corresponding to an image set
         * @param viewID ID Corresponding the a particular view in the image set
         * @return null if Image does not exist in the store
         */
        virtual void loadImage(ImageMan::VBHandle& handle) = 0;

        /**
         * Save an image corresponding to a baseID and a viewID.
         *
         * @param baseID ID Corresponding to an image set
         * @param viewID ID Corresponding the a particular view in the image set
         * @param view View to store
         */
        virtual void saveImage(ImageMan::VBHandle handle) = 0;

        /**
         * Remove an image from the store.
         *
         * @param baseID ID Corresponding to an image set
         * @param viewID ID Corresponding the a particular view in the image set
         */
        virtual void destroyImage(const char* baseID,
            const char* viewID) = 0;

        virtual bool containsImage(const char* baseID, const char* viewID)=0;
    };

};

