#ifndef WRITE_AVI
#define WRITE_AVI
// $Header: /p/graphics/CVS/src/ImageTools/WriteAvi.H,v 1.4 2002/08/09 19:37:02 hupp Exp $
//#ifndef VideoToolsDLL_H
//#include "VideoToolsDLL.H"
//#endif


#include <vfw.h>
// From vfw.h
struct IAVIFile;
typedef IAVIFile* PAVIFILE;

#include "AviInterface.h"

class ITImage;

class VT_API WriteAvi {
	
public:
	
	WriteAvi(const char* filename, int totalFrames, int width, int height,
		int frameRate = 30, bool hasAlpha = false, int frameRateDivisor = 1);
	
	~WriteAvi();
	
	void addFrame(ITImage* i);
	void addAudio(PAVISTREAM);
	void writeAudioStream(LPBYTE, long, PAVISTREAM);
		
	PAVIFILE theAviFile;
	PAVISTREAM theAviStream; //video stream
	PAVISTREAM theAviAudio;	 //audio stream
	PAVISTREAM theCompStream;
	AVISTREAMINFO psi;
	BITMAPINFOHEADER *theBitmap;
	
	
	int frames;
	const char* savefilename;
	
	long position;
	
	long guardY;
	long guardX;
};



// create a new WriteAVI

VT_API WriteAvi* writeAvi(const char* filename, int totalFrames, int width, int height,
						  int frameRate = 30, bool hasAlpha = false, int frameRateDivisor = 1);

VT_API void deleteWriteAvi(WriteAvi*);
VT_API void addFrame(WriteAvi*, ITImage*);



#endif /* WRITE_AVI */

