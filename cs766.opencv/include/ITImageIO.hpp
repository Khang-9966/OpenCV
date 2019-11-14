#ifndef itimageio_h
#define itimageio_h


#ifdef _DEBUG
#define IL_DEBUG
#endif

/* Otherwise you need devil.lib when just using 
	ImageTools								*/
#ifndef IT_BUILD_LIBRARY
#define _IL_BUILD_LIBRARY
#endif

#include "ImageTools.h"
#include "ITImage.h"
#include <string>
#include <vector>

struct imageio {

	ITIMAGE_API static ITImage* read(const std::string& str)
	{	return read(str.c_str());	}

	ITIMAGE_API static ITImage*	read(const char* file);
	ITIMAGE_API static ITImage* readpgm(const char *file);


	ITIMAGE_API static void		write(const ITImage&, const char* file);
	ITIMAGE_API static void		writepgm( const ITImage&, const char *file);
	ITIMAGE_API static void		writeppm( const ITImage&, const char *);


#ifndef SWIG
	ITIMAGE_API	static ITImage*	read(const char* file, int filetype);
    ITIMAGE_API static ITImage* read(void * lump, size_t size);
	ITIMAGE_API static void		write(const ITImage* img, const char* file) 
	{	write(*img, file); }

	ITIMAGE_API static void		write(const ITImage&, const char* file, int filetype);

#endif
    ITIMAGE_API static void readAnalyze(std::vector<ITImage*>& images, const char* hdrFile);
    ITIMAGE_API static void writeAnalyze(const std::vector<ITImage*>& images, const char* hdrFile);
};

#endif // itimageio_h