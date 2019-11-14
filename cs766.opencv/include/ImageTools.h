#ifndef _imagetools_h_
#define _imagetools_h_


#if defined(WIN32) && !defined(SWIG)
#	ifdef IT_BUILD_LIBRARY
#		define ITIMAGE_API	__declspec( dllexport )
#	else
#		define ITIMAGE_API	
#		ifdef  _DEBUG
#			pragma comment(lib, "ImageTools_d.lib")
#		else
#			pragma comment(lib, "ImageTools.lib")
#		endif
#	endif
#else
#	define ITIMAGE_API
#endif



#ifdef __cplusplus
extern "C" {
#endif


#include <ipl.h>


typedef IplImage it_image_t;


#define IT_CHANNEL0_BIT         (1)
#define IT_CHANNEL1_BIT         (2)
#define IT_CHANNEL2_BIT         (3)
#define IT_CHANNEL3_BIT         (4)
#define IT_ALL_CHANNEL_BITS     (0)

typedef enum{

    IT_ERR_NONE = 0,
    IT_ERR_INVALID_FORMAT = 1,
    IT_ERR_INVALID_FILE_FORMAT = 2,
    IT_ERR_INVALID_VALUE = 3,
    IT_ERR_WRITE_INCOMPATIBLE_FORMAT = 4,
    IT_ERR_READ_INCOMPATIBLE_FORMAT = 5,
    IT_ERR_OUT_OF_MEMORY = 6

} it_err_codes_t;

typedef enum {

    IT_AUTO = 0,

    IT_TARGA = 1,
    IT_JPEG = 2,
    IT_RADIANCE = 3,

    IT_NUM_FORMATS = 4

}it_image_format_t;

// for various reasons, these should be in increasing order
// precision, so char is inferior to short is inferior to float.
// this is required because some routines will try to fall back
// to different formats when things don't work, and it assumes
// that things are in this order.
typedef enum {

    IT_UNSIGNED_CHAR = IPL_DEPTH_8U,
    IT_UNSIGNED_SHORT = IPL_DEPTH_16U,
	IT_SIGNED_INT = IPL_DEPTH_32S,
    IT_FLOAT = IPL_DEPTH_32F,

	IT_NUM_CHANNEL_TYPES = 3

} it_type_t;

typedef enum {

    IT_ORIGIN_AUTO = IPL_ORIGIN_TL,
    IT_ORIGIN_UPPER_LEFT = IPL_ORIGIN_TL,
    IT_ORIGIN_LOWER_LEFT = IPL_ORIGIN_BL

} it_origin_t;









/*
================

  Image manipulation stuff

================
*/

// These have been changed to external linkage since they're used in ITImage
void _AllocateImage(it_image_t*);
void _DeallocateImage(it_image_t*);
int _AreCompatible( const it_image_t *src, const it_image_t *dst );


// create and destroy
it_image_t	*itCreateImage( int width, int height, int nchannels, 
                         it_type_t type, it_origin_t origin, int align );
void        itDestroyImage( it_image_t *image );


// reading
it_image_t *itReadImage( const char *filename, it_image_format_t type, int nchannels, it_type_t chantype, 
                       it_origin_t origin, int align );
void        itReadImageInto( const char *filename, it_image_format_t type, it_image_t *image );

// writing
void        itWriteImage( const char *filename, it_image_format_t type,
                       it_image_t *image );


// region of interest
void        itSetROI( it_image_t *image, int x, int y, int w, int h );
void        itGetROI( it_image_t *image, int *x, int *y, int *w, int *h );
void        itClearROI( it_image_t *image );


// channel masks
void        itSetChannelMask( it_image_t *image, int mask );
int         itGetChannelMask( it_image_t *image );




void        itCopyImage( it_image_t *src, it_image_t *dst );
void        itCopyImageInto( it_image_t *src, it_image_t *dst, int x, int y );


void        itFillImage( it_image_t *image, int pattern_length,
                      const void *pattern );

void        *itGetImageRow( it_image_t *image, int row );



// flipping horizontally and vertically
void        itFlipImageHorizontal( it_image_t *img );
void        itFlipImageVertical( it_image_t *img );




// if dst is NULL or the same as src, the src image is converted.
// If dst is NOT NULL, then the conversion is made from src to dst
// and does not modify src.
void        itConvertImage( it_image_t *src, int nchannels, 
                         it_type_t type, int origin, int align );



void        itSwapChannels( it_image_t *img, int channel1, int channel2 );


// crops to ROI.
void        itCropImage( it_image_t *img );


void        itDeinterlaceImage( it_image_t *img1, it_image_t *img2, it_image_t *forward, it_image_t *back );





/* INTEL IPL ROUTINES */

void        itWarpPerspectiveQ( it_image_t src, const double quad[4][2], 
                             int interpolation );
void        itWarpPerspective( it_image_t src, const double coef[3][3], 
                             int interpolation );


void        itScaleBiasImage( it_image_t *src, it_image_t *dst, float scale, float bias );


void        itSquareImage( it_image_t *img );


// dst = op1 + op2
void        itAddImages( it_image_t *op1, it_image_t *op2, it_image_t *dst );


// dst = op1 - op2
void        itSubtractImages( it_image_t *op1, it_image_t *op2, it_image_t *dst );


// dst = op1 * op2
void        itMultiplyImages( it_image_t *op1, it_image_t *op2, it_image_t *dst );


// dst = op1 * scalar + op2
void        itScaleAddImages( float scalar, it_image_t *op1, it_image_t *op2, it_image_t *dst );



// theta in degrees, and positive angles rotate counter clockwise
void        itRotateImage( it_image_t *img, float theta );



void        itShearImage( it_image_t *img, float shear_x, float shear_y, float sihft_x, float shift_y );



void        itStretchImage( it_image_t *img, float scale_x, float scale_y );



// flipping horizontally and vertically
void itFlipImageHorizontal( it_image_t *img );
void itFlipImageVertical( it_image_t *img );














/*
================

  Image collections stuff

================
*/


typedef struct _it_image_set_s {

    int foo;

} it_image_set_t;







it_image_set_t      *CreateImageSet();
void                DestroyImageSet( it_image_set_t *set );


it_image_set_t      *ReadImageSetAVI( const char *filename );

// format is a printf-style format string that will be evalued with
// a single int parameter to generate filenames to load.
it_image_set_t      *ReadImageSetSequence( const char *format, it_image_format_t image_type, 
                                          int min, int step, int num );

it_image_set_t    *ReadImageSetList( int argc, const char *argv[] );




void                WriteImageSetAVI( const char *filename, const it_image_set_t *set );


void                WriteImageSetSequence( const char *format, it_image_format_t image_type,
                                          int min, int step, int num, const it_image_set_t *set );

void                WriteImageSetList( int argc, const char *argv[], int min, int step, 
                                     const it_image_set_t *set );





// inserts the image BEFORE the index given, thus 0 prepends, 
// and last_index + 1 appends.
void                InsertImage( it_image_set_t *set, const it_image_t *image, int index );

// exactly equivalent to InsertImage( set, image, set->last + 1 );
void                AppendImage( it_image_set_t *set, const it_image_t *image );

// exactly equivalent to InsertImage( set, image, 0 );
void                PrependImage( it_image_set_t *set, const it_image_t *image );



// inserts the images in the src set to the destination set BEFORE the given
// index.
void                InsertImagesFromSet( const it_image_set_t *src, it_image_set_t *dst, int index );


// concatenate the image sets.
// exactly equivalent to InsertImagesFromSet( set2, set1, set1->last + 1 );
void                ConcatenateSets( it_image_set_t *set1, const it_image_set_t *set2, int index );



// removes and destroys an image from the set
void                RemoveImage( it_image_set_t *set, int index );




// Also removes an image from the set, but does not destroy it.
it_image_t          *ExtractImage( it_image_set_t *set, int index );




// Returns a pointer to an image in the set (does not change the set)
it_image_t          *GetImageAt( const it_image_set_t *set, int index );


#ifdef __cplusplus
}
#endif


#endif /* _imagetools_h_ */