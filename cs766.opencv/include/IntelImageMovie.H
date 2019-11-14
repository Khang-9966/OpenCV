#ifndef _INTEL_IMAGE_MOVIE_H
#define _INTEL_IMAGE_MOVIE_H
// $Header: /p/graphics/CVS/src/ImageTools/IntelImageMovie.H,v 1.5 2003/06/04 17:30:57 hupp Exp $

//#ifndef VideoToolsDLL_H
//#include "VideoToolsDLL.H"
//#endif

// this class defines an abstraction of a source of frames, much
// simpler than an AVI stream or Quicktime Movie

// for now, it has a naive notion of time in "frames"



#include "AviInterface.h"

#ifdef SWIG
%nodefault
#endif

#ifndef SWIG
class ITImage;
#endif

class VT_API IntelImageMovie {
	
public:
	
	// virtual destructor is key! 
	// Why does this need to be defined?
	virtual ~IntelImageMovie(){}
	// how long is the movie? 
	virtual int nframes() = 0;		   
	
	/* the idea is that this is cached internally, so don't expect it to hang around long */
	virtual ITImage* getFrame(int)=0; 
	
	virtual int height() = 0;		/* how big are the frames */
	virtual int width() = 0;
	virtual int depth() = 0;		/* how many bytes/pixel */
	
};





#ifdef SWIG
%makedefault
#endif

#endif
