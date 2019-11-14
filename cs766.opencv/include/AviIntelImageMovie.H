#ifndef _AVI_INTEL_IMAGE_MOVIE
#define _AVI_INTEL_IMAGE_MOVIE

// $Header: /p/graphics/CVS/src/ImageTools/AviIntelImageMovie.H,v 1.5 2002/08/09 19:36:46 hupp Exp $

//#ifndef VideoToolsDLL_H
//#include "VideoToolsDLL.H"
//#endif



#include "AviInterface.H"
#include "IntelImageMovie.H"


class ITImage;

class VT_API AviIntelImageMovie : public IntelImageMovie {
 public:
  AviIntelImageMovie();			/* make an empty one */
  ~AviIntelImageMovie();		/* destructor is important! */

  // we load the movie in a function so that we can give some error
  // feedback 
  bool loadMovie(const char* filename, bool makeAlpha=false);

  void unloadMovie();


  // key interface for being a movie
  virtual int nframes();
  virtual ITImage *getFrame(int);
  virtual void getFrameNC(int, ITImage *); // decode into an ITImage that is not cache
  virtual int height();		/* how big are the frames */
  virtual int width();
  virtual int depth();		/* how many bytes/pixel */


  // keep the info needed to deal with the AVI
  PAVISTREAM theAvi;			/* the movie itself */
  PGETFRAME  thePGetFrame;		/* the codec instance */
  int sampleCount;				/* a different notion of frames */
  float framesPerSecond;        /* actually "samples" per second */
  int fpsRate;                  /* fspRate/fpsScale = framesPerSecond */
  int fpsScale;                 /* fspRate/fpsScale = framesPerSecond */

  
  /*AUDIO INFO*/
  PAVISTREAM theAviAudio;		/* the audio */
  bool hasAudio;
 
	// fps is public, this is unnecssary
#ifndef SWIG
	float getFPS() { return framesPerSecond; }
#endif
	void getFPS(int &rate, int &scale) { rate = fpsRate; scale = fpsScale; }
	
	// cache a frame
	ITImage* cache;
	
	// original pathname passed into loadmovie
	char* pathname;

};



#endif
