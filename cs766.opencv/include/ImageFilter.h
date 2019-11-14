#pragma once
#include "imdllexp.h"
#include <string>
#include <list>
#include "ImageUtil.h"
#include "ValueBlock.h"
namespace ImageMan {
    class ImageIO;

    class IM_DLL ImageFilter : public Damageable {
    public:
        ImageFilter();
        virtual ~ImageFilter();

        virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst)=0;
        virtual bool init(const char* baseID);
        virtual int getWidth();
        virtual int getHeight();
        virtual int numChannels();
    protected:
#pragma warning(push)
#pragma warning(disable: 4251)
        std::string m_baseID;
#pragma warning(pop)
    };

    namespace filt {
        /**
         * If A|B != 0, sets to 1, else 0
         */
        class IM_DLL Union : public ImageFilter {
        public:
            Union(ImageFilter* srcOne, ImageFilter* srcTwo);
            virtual ~Union();

        public: // ImageFilter overrides
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);

        public:
            ImageFilter *m_one,
                *m_two;
        };

        /**
         * If A&B != 0, sets to 1, else 0
         */
        class IM_DLL Intersection : public ImageFilter {
        public:
            Intersection(ImageFilter* srcOne, ImageFilter* srcTwo);
            virtual ~Intersection();

        public: // ImageFilter overrides
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);

        public:
            ImageFilter *m_one,
                *m_two;
        };

        class IM_DLL ConvKernel {
        public:
            ConvKernel(int width, int height);
            ConvKernel(double* vals, int width, int height);
            ConvKernel(const ConvKernel& rhs);
#ifndef SWIG
            ConvKernel& operator=(const ConvKernel& rhs);
#endif
            ~ConvKernel();
            void setVal(int x, int y, double val);
            int getWidth() const {
                return m_width; }
            int getHeight() const {
                return m_height; }
            double* getVals() const {
                return m_kernel; }
        private:
            double* m_kernel;
            int m_width;
            int m_height;
        };

        /**
         * Performs a convolution
         */
        class IM_DLL Convolve : public ImageFilter {
        public:
//            Convolve(ImageFilter* src);
            Convolve(ImageFilter* src, ConvKernel* kernel);

            virtual ~Convolve();

            void setKernel(const ConvKernel& kernel);

        public: // ImageFilter overrides
            virtual void damage(const char* baseID, const Region& region);
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);

        private:
            VBHandle convolveSep(const Region& dst);

            VBHandle convolve(const Region& dst);

            ImageFilter* m_src;
            int m_kWidth, m_kHeight;
            double *m_kernel,
                *m_kSepW,
                *m_kSepH;
            bool m_bSeparable;
        };

        /**
         * Creates a false color image
         */
        class IM_DLL FalseColor : public ImageFilter {
        public:
            FalseColor(ImageFilter* src);
            FalseColor(ImageFilter* src, double min, double blue, double green,
                double yellow, double orange, double red, double max);
            virtual ~FalseColor();
            void setMin(double min);
            void setBlue(double blue);
            void setGreen(double green);
            void setYellow(double yellow);
            void setOrange(double orange);
            void setRed(double red);
            void setMax(double max);

            virtual int numChannels();
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);
        private:
            double m_blue,
                m_green,
                m_yellow,
                m_orange,
                m_red,
                m_min,
                m_max;
            ImageFilter* m_src;
        };

        class IM_DLL Loader : public ImageFilter {
        public:
            Loader(ImageMan::VBHandle handle);
            virtual ~Loader();
            virtual bool init(const char* baseID);
            virtual int getWidth();
            virtual int getHeight();
            virtual int numChannels();
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);
        private:
            int m_width;
            int m_height;
            int m_channels;
            VBHandle m_handle;
        };

        class IM_DLL CreateEmptyCopy : public ImageFilter {
        public:
            CreateEmptyCopy(ImageMan::VBHandle handle);
            virtual ~CreateEmptyCopy();
            virtual bool init(const char* baseID);
            virtual int getWidth();
            virtual int getHeight();
            virtual int numChannels();
            virtual ImageMan::VBHandle calcRegion(const ImageMan::Region& dst);
        private:
            int m_width;
            int m_height;
            int m_channels;
            VBHandle m_handle;
            VBHandle m_blank;
        };
    };
};
