#ifndef AVIINTERFACE_H
#define AVIINTERFACE_H
// $Header: /p/graphics/CVS/src/ImageTools/AviInterface.H,v 1.6 2003/06/04 21:41:16 hupp Exp $
//#ifndef VideoToolsDLL_H
//#include "VideoToolsDLL.H"
//#endif

// interface to video for windows
// idea: keep track of what is/isn't opened so that they can be opened
// and closed once

// warning - my versions of the routines have the same names, but 
// different capitalization

//#include <vfw.h>
#ifndef SWIG
#if defined(WIN32) 
#define VT_API __declspec( dllexport )
#else
#define VT_API
#endif
#else
#define VT_API
#endif

#ifndef VT_BUILD_LIBRARY
#	ifdef  _DEBUG
#		pragma comment(lib, "VideoTools_d.lib")
#	else
#		pragma comment(lib, "VideoTools.lib")
#	endif
#endif

// make "blind" version of the routines so we can prototype them
// without including all the windows slime
struct IGetFrame;
typedef IGetFrame* PGETFRAME;

struct IAVIStream;
typedef IAVIStream* PAVISTREAM;

struct tagBITMAPINFOHEADER;
typedef struct tagBITMAPINFOHEADER* LPBITMAPINFOHEADER;




/*
#ifndef SWIG

// trick - we need to have something that we are sure to run at the
// end of the program
class VT_API AviInterfaceState {
	
public:
	~AviInterfaceState();
};

extern VT_API AviInterfaceState theAviInterfaceState;

#endif
*/


VT_API char* errToString(long);

// this does the "FileInit"
// however, only the first one actually does the init!
VT_API void AviFileInit();				/* notice capitalization! */
VT_API void AviFileExit();				/* we're done */

// this takes care of the "get frame open" problem
// it is really important that every opened one gets closed,
// otherwise some codecs really barf (Win98 kernel panic)
// unfortunately, its unclear what a "PGETFRAME" is to blind typedef!

// our own version that does some logging
VT_API PGETFRAME AviStreamGetFrameOpen(PAVISTREAM pavi,
									   LPBITMAPINFOHEADER lpbiWanted);
VT_API void AviStreamGetFrameClose(PGETFRAME);

// verbosely print all the AVIStream info
VT_API void printAviStreamInfo(PAVISTREAM);


#endif
