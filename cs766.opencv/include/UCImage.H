#ifndef _UCImage_H_
#define _UCImage_H_

/*! \file UCImage.H
	\brief Single chanel unsigned char images

	this file contains a special case of an ITImage - a single chanel,
	single unsigned byte image. this is the most important special
	case for many things, so this makes life easier in lots of ways

	i can't believe i didn't do this earlier

	gleicher 11/13/2004
**/

#include "ImageTools.h"
#include "ITDebug.h"
#include "ITImage.h"

//! An unsigned char 1 chanel image
class ITIMAGE_API UCImage : public ITImage {
public:
  /// note: we only support the common case
  // if you want an uncommon case, use the parent
  inline UCImage(int width, int height) 
	: ITImage(width, height, 1, IPL_DEPTH_8U) {};

  /* we only replace things that are different, or hard to do in
	 general */
  unsigned char* getRow(const int i) const {
		return ( (unsigned char*)imageData + i*widthStep);
  };
  unsigned char * getImageData() const {
	return (unsigned char*) imageData; 
  }
  inline unsigned char & operator()(const int x, const int y) {
	return ( getRow(y)[x] );
  }

  void fill(const unsigned char);
  
};


//! the "unsafe" cast - does check
inline UCImage* itImageASucImage(ITImage* i) {
  if (i && (i->depth != IPL_DEPTH_8U) && (i->nChannels == 1))
	return (UCImage*) i;
  else {
	IT_WARN("Bad ITImage to itImageASucImage");
	return 0;
  }
};
#endif
