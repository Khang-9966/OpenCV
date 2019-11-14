#ifndef _ITImage_H_
#define _ITImage_H_

#include <ostream>
#include <iostream>
#include <string>
#include <sstream>

#include "ImageTools.h"
#include "ITDebug.h"


#define RUNTIME_ERROR(arg) IT_WARN(arg)

#ifndef SWIG
class ITIMAGE_API ITImage : public IplImage 
#else
class ITImage
#endif
{

	void		preinit();
	void		postinit();
	
	void		allocate(int width, int height, int depth, int chan, bool clear);
	void		doCopy(const ITImage&);

	int			bytesPerChannel;

public:

				ITImage();
				ITImage(const ITImage&);

				// Image Construction: nchannels should be limited to
				// 1-4 (maybe change this to a specific type? 
				// Type is either IT_UNSIGNED_CHAR, IT_UNSIGNED_SHORT,
				// IT_FLOAT or the IPL_DEPTH_ equilivant 
				// FIXME:  Correct type and channels to defend against
				// "outrageous" or incorrect values! 
				ITImage(int width, int height, int nchannels, int type, bool bClear=true);
				ITImage(IplImage src);
				~ITImage(); 
#ifndef SWIG
	ITImage&	operator=(const ITImage&);
#endif

	// Just for SWIG - we can't overload the "=" and need a way of
	// making "deep" copies when using pyythong 
	void		copy(ITImage *dst) const { *dst = *this; }

	ITImage*	emptyCopy() const
	{	return new ITImage(getWidth(), getHeight(), getNumChannels(), getDepth());	}

	
	// accessors not in the C interface:
	int			getWidth()	const		{ return width; }
	int			getHeight() const		{ return height; }
	int			getSpan()	const		{ return widthStep; }
	int			getNumChannels() const	{ return nChannels; }
	int			getOrigin()		const	{ return origin; }
	int			getAlignment()	const	{ return align; }
	int			getAlphaChannel() const { return alphaChannel; }
	std::string getColorModel() const	{ return std::string(colorModel); }
	std::string getChannelSeq() const	{ return std::string(channelSeq); }
	int			getDepth()	const		{ return depth; }
	int			getBPC()	const		{ return bytesPerChannel; }


	// Region of Interest Operations
	void		setROI( int x, int y, int w, int h );
	void		setROI( IplROI r)		{ setROI(r.xOffset, r.yOffset, r.width, r.height); }
	void		setROI(ITImage* i)		{ maskROI = i; }
	void		getROI( int *x, int *y, int *w, int *h ) const;
	IplROI		getROI() const;			
	void		clearROI()				{ if(roi){ delete roi; roi = 0; }}
	
	// Channel mask operations
	void		setChannelMask(int);
	int			getChannelMask() const	{ return roi ? roi->coi : 0; }
	
	// self->CopyImage(dst) would put "self" into dst
	void		copyInto( ITImage *dst, int x, int y );
	
	void		patternFill( int pattern_length, const void *pattern );
	
	
	// Returns a pointer to "row"
	// IMPORTANT: When using this, you must cast getRow() to the correct image format type, i.e. "unsigned char *"
	// How could you do this with templates to get the return typing right?
	inline void*	getRow(int row) const {
		return (void*)( (char*)imageData + row*widthStep);
	}

  // returning a void* is totally useless in SWIG
#ifndef SWIG
	inline void*	operator()(const int x, const int y, const int channel=0) const {
		return (void*)( (char*)getRow(y) + x*bytesPerChannel*nChannels + channel*bytesPerChannel);
	}
#endif

	/// get a byte at a pixel
	unsigned char getByte(const int x, const int y, const int c=1) const
	{	return * ((unsigned char*) ((*this)(x,y,c)));	}

	// Returns a pointer to the entire image
	void*		getImageData() const	{ return imageData; }
	
	// flipping horizontally and vertically
	void		flipHorizontal();
	void		flipVertical();
	
	// Changes the number of channels and type of the image
	// NOTE: This is only here for "legacy" code and should not really be used
	// use iplConvert( IplImage *src, IplImage *dst ) instead
	// Eventually we should change this to just be a call to IPL and make it a "psuedo" inplace operation
	int			convert( int nchannels, int type); 
	
	void		swapChannels( int channel1, int channel2 );
	
	// crops to ROI.
	void		crop();
#ifndef SWIG
	friend ITIMAGE_API std::ostream& operator<<(std::ostream&, const ITImage&);
#endif
};

#ifndef SWIG
ITIMAGE_API std::ostream& operator<<(std::ostream&, const ITImage&);
ITIMAGE_API std::ostream& operator<<(std::ostream&, const IplROI&);
#endif

typedef struct {
	
	int iplId;
	const char* name;
	int numBits;
	bool isSigned;
	

} depthInfo;

const depthInfo depthMap[] = {
	
	{ IPL_DEPTH_1U,		"IPL_DEPTH_1U",		1,	false	},
	{ IPL_DEPTH_8U,		"IPL_DEPTH_8U",		8,	false	},
	{ IPL_DEPTH_8S,		"IPL_DEPTH_8S",		8,	true	},
	{ IPL_DEPTH_16U,	"IPL_DEPTH_16U",	16,	false	},
	{ IPL_DEPTH_16S,	"IPL_DEPTH_16S",	16,	true	},
	{ IPL_DEPTH_32S,	"IPL_DEPTH_32S",	32,	true	},
	{ IPL_DEPTH_32F,	"IPL_DEPTH_32F",	32, true	},
	{ 0,				"Unknown Depth",	0,	false	}
	
};

// This isn't necessary, use getByte
#ifdef SWIG
// swig can't overload the () operator from addmethods
%extend ITImage {
  char e(int x, int y, int c=0) {
	return *((char*) ( self->operator() (x,y,c)));
  }
};
#endif

ITIMAGE_API depthInfo getDepthInfo(int ipldepth);

/// force the color model to be a certain string
/** note: this does not do any conversions! it simply replaces the
	color model strings so that ImageTools THINKS your image is
	something other than what it is. This makes it possible to
	load/save different images 
	Note: this updates the color model and the channel sequence,
	although its not obvious what the difference is
**/
ITIMAGE_API void forceColorModel(ITImage*, const char*);


#endif /* _ITImage_H_ */


