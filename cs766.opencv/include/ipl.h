/*
//
//               INTeL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1995 Intel Corporation. All Rights Reserved.
//
//
//  Purpose: IPL Common Header file
//
*/

#if !defined (__IPL_H__) || defined (_OWN_BLDPCS)
#define __IPL_H__


#if defined( _WIN32 )
  #define __STDCALL __stdcall
  #define __CDECL __cdecl
  #define __INT64 __int64
#else
  #define __STDCALL
  #define __CDECL
  #define __INT64 long long
#endif

#if defined( _WIN32 )
  #define __INLINE static __inline
#elif defined( __ICC )
  #define __INLINE static __inline
#elif defined( __GNUC__ )
  #define __INLINE static __inline__
#else
  #define __INLINE static
#endif

#if defined( _VXWORKS )
    #include "types/vxTypesOld.h"
#endif

#if !defined (_INC_WINDOWS) && !defined (_WINDOWS_)
 #if !defined (IPL_WINDOWS) && defined( _WIN32 )
  #include <windows.h> /* include wingdi.h for BITMAPINFOHEADER declaration*/
 #else
  #include "iplwind.h" /* smaller file */
 #endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------  IPLibrary call conversion  ----------------------*/
#if !defined IPLAPI
  #if defined(IPL_W32DLL)
    #ifndef __BORLANDC__
      #define IPLAPI(type,name,arg) \
                       extern __declspec(dllimport) type __STDCALL name arg;
    #else
      #define IPLAPI(type,name,arg)    extern type _import __STDCALL name arg;
    #endif
  #else
    #define IPLAPI(type,name,arg)            extern type __STDCALL name arg;
  #endif
#endif



/*==========================================================================
       Section: IPLibrary Definitions
 ==========================================================================*/

#define IPL_DEPTH_SIGN 0x80000000               
#define IPL_DEPTH_MASK 0x7FFFFFFF               

#define IPL_DEPTH_1U     1
#define IPL_DEPTH_8U     8
#define IPL_DEPTH_16U   16
#define IPL_DEPTH_32F   32

#define IPL_DEPTH_8S  (IPL_DEPTH_SIGN| 8)
#define IPL_DEPTH_16S (IPL_DEPTH_SIGN|16)
#define IPL_DEPTH_32S (IPL_DEPTH_SIGN|32)

#define IPL_DATA_ORDER_PIXEL  0
#define IPL_DATA_ORDER_PLANE  1

#define IPL_ORIGIN_TL 0
#define IPL_ORIGIN_BL 1

#define IPL_ALIGN_4BYTES   4
#define IPL_ALIGN_8BYTES   8
#define IPL_ALIGN_16BYTES 16
#define IPL_ALIGN_32BYTES 32

#define IPL_ALIGN_DWORD   IPL_ALIGN_4BYTES
#define IPL_ALIGN_QWORD   IPL_ALIGN_8BYTES

#define IPL_GET_TILE_TO_READ   1
#define IPL_GET_TILE_TO_WRITE  2
#define IPL_RELEASE_TILE       4

#define IPL_LUT_LOOKUP 0
#define IPL_LUT_INTER  1


 
/*==========================================================================
     Section: IPLibrary Types
 ==========================================================================*/

#if !defined _OWN_BLDPCS

  typedef int  IPLStatus;
  typedef unsigned char   uchar;

  #if defined( linux )
    #include <sys/types.h>
  #elif defined( _VXWORKS )
    typedef unsigned int    uint; 
  #else
    typedef unsigned short  ushort;
    typedef unsigned int    uint; 
  #endif

typedef struct _IplImage {
    int             nSize;              /* size of iplImage struct         */
    int             ID;                 /* version                         */
    int             nChannels;
    int             alphaChannel;
    int             depth;              /* pixel depth in bits             */
    char            colorModel[4];
    char            channelSeq[4];
    int             dataOrder;
    int             origin;
    int             align;              /* 4 or 8 byte align               */
    int             width;
    int             height;
    struct _IplROI *roi;
    struct _IplImage
                   *maskROI;            /* poiner to maskROI if any        */
    void           *imageId;            /* use of the application          */
    struct
      _IplTileInfo *tileInfo;           /* contains information on tiling  */
    int             imageSize;          /* useful size in bytes            */
    char           *imageData;          /* pointer to aligned image        */
    int             widthStep;          /* size of aligned line in bytes   */
    int             BorderMode[4];      /*                                 */
    int             BorderConst[4];     /*                                 */
    char           *imageDataOrigin;    /* ptr to full, nonaligned image   */
} IplImage;

typedef struct _IplROI {
    int             coi;
    int             xOffset;
    int             yOffset;
    int             width;
    int             height;
} IplROI;



/* /////////////////////////////////////////////////////////////////////////
// type IplCallBack
// Purpose:    Type of functions for access to external manager of tile
// Parameters: 
//   img           - header provided for the parent image
//   xIndex,yIndex - indices of the requested tile. They refer to the tile
//                   number not pixel number, and count from the origin at (0,0)
//   mode          - one of the following:
//    IPL_GET_TILE_TO_READ  - get a tile for reading;
//                            tile data is returned in "img->tileInfo->tileData",
//                            and must not be changed
//    IPL_GET_TILE_TO_WRITE - get a tile for writing;
//                            tile data is returned in "img->tileInfo->tileData"
//                            and may be changed;
//                            changes will be reflected in the image
//    IPL_RELEASE_TILE      - release tile; commit writes
//
// Notes: Memory pointers provided by a get function will not be used after the
//        corresponding release function has been called.
//
*/

typedef void (__STDCALL *IplCallBack)(const IplImage* img, int xIndex,
                                                          int yIndex, int mode);

typedef struct _IplTileInfo {
    IplCallBack     callBack;         /* callback function                  */
    void           *id;               /* additional identification field    */
    char           *tileData;         /* pointer on tile data               */
    int             width;            /* width of tile                      */
    int             height;           /* height of tile                     */
} IplTileInfo;

typedef struct _IplLUT {
    int             num;
    int            *key;
    int            *value;
    int            *factor;
    int             interpolateType;
} IplLUT;

typedef struct _IplColorTwist {
    int           data[16];
    int           scalingValue;
} IplColorTwist;


typedef struct _IplConvKernel {
    int           nCols;
    int           nRows;
    int           anchorX;
    int           anchorY;
    int          *values;
    int           nShiftR;
} IplConvKernel;

typedef struct _IplConvKernelFP {
    int           nCols;
    int           nRows;
    int           anchorX;
    int           anchorY;
    float        *values;
} IplConvKernelFP;

typedef enum {
    IPL_PREWITT_3x3_V=0,
        IPL_PREWITT_3x3_H,
        IPL_SOBEL_3x3_V,   /* vertical   */
        IPL_SOBEL_3x3_H,   /* horizontal */
        IPL_LAPLACIAN_3x3,
        IPL_LAPLACIAN_5x5,
        IPL_GAUSSIAN_3x3,
        IPL_GAUSSIAN_5x5,
        IPL_HIPASS_3x3,
        IPL_HIPASS_5x5,
        IPL_SHARPEN_3x3
} IplFilter;

/*-----------------  IplMomentState Structure Definition  -----------------*/
typedef struct {   
    double scale;  /* value to scale (m,n)th moment */
    double value;  /* spatial (m,n)th moment        */
} ownMoment;

typedef ownMoment IplMomentState[4][4];



/*==========================================================================
      Section: Wavelet transform constants and types.
 ==========================================================================*/
      

/*--------------------  Types of wavelet transforms.  ---------------------*/

typedef enum {
    IPL_WT_HAAR,
    IPL_WT_DAUBLET,
    IPL_WT_SYMMLET,
    IPL_WT_COIFLET,
    IPL_WT_VAIDYANATHAN,
    IPL_WT_BSPLINE,
    IPL_WT_BSPLINEDUAL,
    IPL_WT_LINSPLINE,
    IPL_WT_QUADSPLINE,
    IPL_WT_TYPE_UNKNOWN
} IplWtType;

/*-----------------------  Filters symmetry type.  ------------------------*/
typedef enum {
    IPL_WT_SYMMETRIC,
    IPL_WT_ANTISYMMETRIC,
    IPL_WT_ASYMMETRIC,
    IPL_WT_SYMM_UNKNOWN
} IplWtFiltSymm;


/*---------------------  Filter bank orthogonality.  ----------------------*/
typedef enum {
    IPL_WT_ORTHOGONAL,
    IPL_WT_BIORTHOGONAL,
    IPL_WT_NOORTHOGONAL,
    IPL_WT_ORTH_UNKNOWN
} IplWtOrthType;


/*--------------------------  Filter structure  ---------------------------*/
typedef struct {
    float        *taps;             /* filter taps                         */
    int           length;           /* length of filter                    */
    int           offset;           /* offset of filter                    */
    IplWtFiltSymm symmetry;         /* filter symmetry property            */
} IplWtFilter;


/*---------------  Wavelet functions interchange structure  ---------------*/
typedef struct {
    IplWtType     type;             /* type of wavelet transform           */
    int           par1;             /* first param.  (transform order)     */
    int           par2;             /* second param. (only for biorth. tr.)*/
    IplWtOrthType orth;             /* orthogonality property              */
    IplWtFilter   filtDecLow;       /* low-pass decomposition filter       */
    IplWtFilter   filtDecHigh;      /* high-pass decomposition filter      */
    IplWtFilter   filtRecLow;       /* low-pass reconstruction filter      */
    IplWtFilter   filtRecHigh;      /* high-pass reconstruction filter     */
} IplWtKernel;



/*---------------------  Noise generators structure  ----------------------*/
typedef enum {
    IPL_NOISE_UNIFORM,
    IPL_NOISE_GAUSSIAN
} IplNoise;

typedef struct _IplNoiseParam {
    IplNoise     noise;
    unsigned int seed;
    int          lowInt;
    int          highInt;
    float        lowFlt;
    float        highFlt;
} IplNoiseParam;



/*==========================================================================
      Section: User function types.
 ==========================================================================*/

/* /////////////////////////////////////////////////////////////////////////
// type IplUserFunc
// Purpose:    Type of callback functions for user point operation. 
//             Provides user to write his own code to process each channel of
//             srcImage pixel. This function would be passed to iplUserProcess 
//             function as its last parameter.
// Parameters: 
//   src         - value of src image pixel's channel converted to integer
// Returns:        value of dst image pixel's channel. You wouldn't support 
//                 value saturatation, it will be done by iplUserProcess function
//
// Notes: For more information see iplUserProcess function description
//
*/
typedef int   (__STDCALL *IplUserFunc)(int src);

/* /////////////////////////////////////////////////////////////////////////
// type IplUserFuncFP
// Purpose:    Type of callback functions for user point operation. 
//             Provides user to write his own code to process each channel
//             of srcImage pixel. This function would be passed to 
//             iplUserProcessFP function as its last parameter.
// Parameters: 
//   src         - value of src image pixel's channel converted to float
// Returns:        value of dst image pixel's channel. You wouldn't support 
//                 value saturatation for integer data types, it will be done 
//                 by iplUserProcessFP function
//
// Notes: For more information see iplUserProcessFP function description
//
*/
typedef float (__STDCALL *IplUserFuncFP)(float src);

/* /////////////////////////////////////////////////////////////////////////
// type IplUserFuncPixel
// Purpose:    Type of callback functions for user point operation. 
//             Provides user to write his own code to process all channels 
//             of srcImage pixel simultaneously. This function would be 
//             passed to iplUserProcessPixel function as its last parameter.
//             Function may call IPL_ERROR to set IplError status.
// Returns:  None
// Parameters: 
//   srcImage  - src image header to access image depth and number of channels
//   srcPixel  - pointer to array of src image pixel values. 
//               Function ought to convert this pointer to an array of src depth.               
//   dstImage  - dst image header to access image depth and number of channels
//   dstPixel  - pointer to array of dst image pixel values.
//               Function ought to convert this pointer to an array of dst depth.               
//
// Notes: For more information see iplUserProcessPixel function description
//
*/
typedef void  (__STDCALL *IplUserFuncPixel)(IplImage* srcImage, void* srcPixel, 
                                            IplImage* dstImage, void* dstPixel);

#endif

/*==========================================================================
      Section: Image Creation Functions
 ==========================================================================*/

#define IPL_BORDER_CONSTANT   0
#define IPL_BORDER_REPLICATE  1
#define IPL_BORDER_REFLECT    2
#define IPL_BORDER_WRAP       3

/*---  Indexes to access IplImage.BorderMode[],IplImage.BorderConst[]  ----*/
#define IPL_SIDE_TOP_INDEX    0
#define IPL_SIDE_BOTTOM_INDEX 1
#define IPL_SIDE_LEFT_INDEX   2
#define IPL_SIDE_RIGHT_INDEX  3

/*----------  values of argument of iplSetBorderMode(,,border,)  ----------*/
#define IPL_SIDE_TOP        (1<<IPL_SIDE_TOP_INDEX)
#define IPL_SIDE_BOTTOM     (1<<IPL_SIDE_BOTTOM_INDEX)
#define IPL_SIDE_LEFT       (1<<IPL_SIDE_LEFT_INDEX)
#define IPL_SIDE_RIGHT      (1<<IPL_SIDE_RIGHT_INDEX)
#define IPL_SIDE_ALL \
     (IPL_SIDE_RIGHT | IPL_SIDE_TOP | IPL_SIDE_LEFT | IPL_SIDE_BOTTOM)


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSetBorderMode
// Purpose:    
// Returns:    
// Parameters: 
//      mode  - The following modes are supported:
//              IPL_BORDER_CONSTANT  -  The value constVal will be used for 
//                                      all pixels.
//              IPL_BORDER_REPLICATE -  The last row or column will be 
//                                      replicated for the border.
//              IPL_BORDER_REFLECT   -  The last n rows or columns will be 
//                                      reflected in reverse order to create 
//                                      the border.
//              IPL_BORDER_WRAP      -  The required border rows or columns 
//                                      are taken from the opposite side of  
//                                      the image.
//      border   - The side that this function is called for. Can be an OR of
//                 one or more of the following four sides of an image:
//                  IPL_SIDE_TOP     -  Top side.
//                  IPL_SIDE_BOTTOM  -  Bottom side.
//                  IPL_SIDE_LEFT    -  Left side.
//                  IPL_SIDE_RIGHT   -  Right side.
//                  IPL_SIDE_ALL     -  All sides
//                 If  no mode has been set for a side, the default 
//                 IPL_BORDER_CONSTANT
//      constVal - The value to use for the border when the mode is set
//                 to IPL_BORDER_CONSTANT.
//
// Notes:      
*/

IPLAPI(void,iplSetBorderMode,(IplImage *src,int mode,int border,int constVal))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateImageHeader
// Purpose:    Creates an IPL image header according to the specified 
//             attributes.
// Returns:    The newly constructed IPL image header.
// Parameters: 
//  nChannels     - Number of channels in the image.
//  alphaChannel  - Alpha channel number (0 if no alpha channel in image).
//  depth         - Bit depth of  pixels. Can be one of
//                      IPL_DEPTH_1U,
//                      IPL_DEPTH_8U,
//                      IPL_DEPTH_8S,
//                      IPL_DEPTH_16U,
//                      IPL_DEPTH_16S,
//                      IPL_DEPTH_32S.
//                      IPL_DEPTH_32F.
//  colorModel    - A four character array describing the color model,
//                  e.g. "RGB", "GRAY", "MSI" etc.
//  channelSeq    - The sequence of channels in the image,
//                  e.g. "BGR" for an RGB image.
//  dataOrder     - IPL_DATA_ORDER_PIXEL or IPL_DATA_ORDER_PLANE.
//  origin        - The origin of the image.
//                  Can be IPL_ORIGIN_TL or IPL_ORIGIN_BL.
//  align         - Alignment of image data.
//                  Can be IPL_ALIGN_4BYTES (IPL_ALIGN_DWORD) or 
//                  IPL_ALIGN_8BYTES (IPL_ALIGN_QWORD) or 
//                  IPL_ALIGN_16BYTES IPL_ALIGN_32BYTES.
//  width         - Width of  the image in pixels.
//  height        - Height of  the image in pixels.
//  roi           - Pointer to an ROI (region of interest) structure.
//                  This can be NULL (implying a region of interest comprising
//                  all channels and the entire image area).
//  maskROI       - Pointer on mask image
//  imageId       - use of the application
//  tileInfo      - contains information on tiling
//
// Notes:      
*/

IPLAPI(IplImage*, iplCreateImageHeader,
               (int   nChannels,  int     alphaChannel, int     depth,
                char* colorModel, char*   channelSeq,   int     dataOrder,
                int   origin,     int     align,
                int   width,      int   height, IplROI* roi, IplImage* maskROI,
                void* imageId,    IplTileInfo* tileInfo))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAllocateImage, iplAllocateImageFP
// Purpose:    Allocates memory for image data according to the
//             specified header.
// Returns:    
// Parameters: 
//  image     - An IPL image header with a NULL image data pointer.
//              The image data pointer will be set to newly allocated
//              image data memory after calling this function.
//  doFill    - Use a 0 to leave the pixel data uninitialized.
//              Use a not 0 to initialized the pixel data of fillValue
//  fillValue - The initial value to use for pixel data.
//
// Notes:      
*/

IPLAPI(void, iplAllocateImage,(IplImage* image, int doFill, int fillValue))
IPLAPI(void, iplAllocateImageFP,(IplImage* image, int doFill, float fillValue))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateImageJaehne
// Purpose:    Creates a gray (one channel) test image
// Returns:    IPL image or NULL
// Parameters: 
//    depth  - depth of the image to be created.
//             All IPL depths are possible including 32f.
//             For the 32f depth a data range is [0..1)
//    width  - width of the image to be created
//    height - height of the image to be created
//
// Notes:   This test image was seen in
//          B.Jaehne, Digital Image Processing, third edition, 1995   
*/

IPLAPI(IplImage*, iplCreateImageJaehne, ( int depth, int width, int height ))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCloneImage
// Purpose:    Creates a clone of an image
// Returns:    IPL image or NULL
// Parameters: img - image to be cloned.
//             
// Notes:      The function creates a copy of an image img including
//             the field roi by copying. The following fields of the
//             created image are set by function
//             ID = 0, imageId = NULL, maskROI = NULL, tileInfo = NULL      
//             All IPL depths are possible including 32f.
*/

IPLAPI(IplImage*, iplCloneImage, ( const IplImage* img ) )


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeallocateHeader
// Purpose:    deallocate IPL header
// Returns:    
// Parameters: 
//
// Notes:      if image->imageData!=NULL, then first frees imageData
*/

IPLAPI(void, iplDeallocateHeader,(IplImage* image))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeallocateImage
// Purpose:    Deallocates (frees) memory for image data pointed to in
//             the image header.
// Returns:    
// Parameters: 
//     image - An IPL image header with a pointer
//             to allocated image data memory. 
//
// Notes:      The image data pointer will be set to NULL after this 
//             function executes.
*/

IPLAPI(void, iplDeallocateImage,(IplImage* image))


#define IPL_IMAGE_HEADER 1
#define IPL_IMAGE_DATA   2
#define IPL_IMAGE_ROI    4
#define IPL_IMAGE_TILE   8
#define IPL_IMAGE_MASK  16
#define IPL_IMAGE_ALL (IPL_IMAGE_HEADER|IPL_IMAGE_DATA|\
                       IPL_IMAGE_TILE|IPL_IMAGE_ROI|IPL_IMAGE_MASK)
#define IPL_IMAGE_ALL_WITHOUT_MASK (IPL_IMAGE_HEADER|IPL_IMAGE_DATA|\
                       IPL_IMAGE_TILE|IPL_IMAGE_ROI)

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeallocate
// Purpose:    Deallocates or frees memory for image header or data or
//             mask ROI or rectangular ROI, etc or all.
// Returns:    
// Parameters: 
//  image    - An IPL image header
//  flag     - what item to free:
//     IPL_IMAGE_HEADER - free header structure
//     IPL_IMAGE_DATA   - free image data, set pointer to NULL
//     IPL_IMAGE_ROI    - free image roi, set pointer to NULL
//     IPL_IMAGE_TILE   - free image tile, set pointer to NULL
//     IPL_IMAGE_MASK   - free image maskROI, set pointer to NULL
//     IPL_IMAGE_ALL    - free image data, roi, header, maskROI, tile
//     IPL_IMAGE_ALL_WITHOUT_MASK
//                      - as well as IPL_IMAGE_ALL, but maskROI does not 
//                        release
//
// Notes:      
*/

IPLAPI(void, iplDeallocate,(IplImage* image, int flag))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateROI
// Purpose:    Allocates and sets the region of interest (ROI) structure.
// Returns:    Newly constructed ROI structure.
// Parameters: 
//      coi      - The channel region of interest. It can be set to 0 
//                 (for all channels) or a specific channel number.
//      xOffset,
//      yOffset  - The offset from the origin of the rectangular region.
//      height,
//      width    - The size of the rectangular region.
//
// Notes:      
*/

IPLAPI(IplROI *,iplCreateROI,(int coi,    int xOffset, int   yOffset,
                              int width, int height ))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSetROI
// Purpose:    Sets the IplROI structure fields.
// Returns:    
// Parameters: 
//      roi      - The ROI structure to modify.
//      coi      - The channel region of interest. It can be set to 0 
//                 (for all channels) or a specific channel number.
//      xOffset,
//      yOffset  - The offset from the origin of the rectangular region.
//      height,
//      width    - The size of the rectangular region.
//
// Notes:      
*/

IPLAPI(void, iplSetROI,(IplROI*   roi,      int coi,
                        int       xOffset,  int yOffset,
                        int width,          int height))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeleteROI
// Purpose:    Deletes ROI structure
// Returns:    
// Parameters: 
//      roi    The ROI structure to be deleted. 
//
// Notes:      
*/

IPLAPI(void, iplDeleteROI,(IplROI* roi))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateTileInfo
// Purpose:    Creates the IplTileInfo structure.
// Returns:    Newly constructed TileInfo structure.
// Parameters: 
//      callBack           callback function
//      id                 additional identification field
//      width              width of tile
//      height             height of tile
//
// Notes:      
*/

IPLAPI(IplTileInfo*, iplCreateTileInfo, ( IplCallBack  callBack,
                                          void* id, int width, int height ))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSetTileInfo
// Purpose:    Sets attributes for an existing IplTileInfo structure.
// Returns:    
// Parameters: 
//      tileInfo           The TileInfo structure to modify.
//      callBack           callback function
//      id                 additional identification field
//      width              width of tile
//      height             height of tile
//
// Notes:      
*/

IPLAPI(void, iplSetTileInfo, ( IplTileInfo* tileInfo,
                   IplCallBack  callBack, void* id, int width, int height ))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeleteTileInfo
// Purpose:    Deletes the IplTileInfo structure.
// Returns:    
// Parameters: 
//      tileInfo - The pointer to the IplTileInfo structure.
//
// Notes:      
*/

IPLAPI(void, iplDeleteTileInfo, (IplTileInfo* tileInfo))



/*==========================================================================
      Section: Windows* DIB Conversion Functions
 ==========================================================================*/
 
/* /////////////////////////////////////////////////////////////////////////
// Name:       iplTranslateDIB
// Purpose:    Translates a DIB image into an IPL image.
// Returns:    The constructed IPL image.
// Parameters: 
//      dib       - The DIB image.
//      cloneData - A boolean, result of work of the function. If true, 
//                  the image data pointer in IPL image is made to point 
//                  to the DIB image data. Otherwise the DIB image data 
//                  was converting to the IPL image data.
//
// Notes:      
*/

IPLAPI(IplImage*, iplTranslateDIB,(BITMAPINFOHEADER* dib, BOOL* cloneData))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvertFromDIB
// Purpose:    Converts a DIB image to an IPL image with specified attributes.
// Returns:    
// Parameters: 
//      dib   - The input DIB image.
//      image - The IPL image header with specified attributes.
//              
// Notes:       If the data pointer is NULL, image data memory
//              will be allocated and the pointer set to it.
*/

IPLAPI(void, iplConvertFromDIB,(BITMAPINFOHEADER* dib, IplImage* image))

/*----------  Consts of Palette conversion for iplConvertToDIB*  ----------*/
#define IPL_PALCONV_NONE      0
#define IPL_PALCONV_POPULATE  1
#define IPL_PALCONV_MEDCUT    2

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvertToDIB
// Purpose:    Converts an IPL image to a DIB image with specified attributes.
// Returns:    
// Parameters: 
//   image  - The input IPL image.
//   dib    - The output DIB image.
//   dither - The dithering algorithm to use if applicable.
//            Dithering will be done if  the bit depth in the DIB
//            is less than that of the IPL image.
//            The following algorithms are supported for all dither 
//            type (see iplReduceBits).
//   paletteConversion -
//      Applicable when the DIB is a palette image.
//      Specifies the palette algorithm to use when converting
//      the IPL absolute color image.
//      The following options are supported:
//          IPL_PALCONV_NONE     - The existing palette in the DIB is used.
//          IPL_PALCONV_POPULATE - The popularity palette conversion
//                                 algorithm is used.
//          IPL_PALCONV_MEDCUT   - The median cut algorithm palette 
//                                 conversion algorithm is used.
//
// Notes:      
*/

IPLAPI(void, iplConvertToDIB,(IplImage* image, BITMAPINFOHEADER* dib, 
                              int dither, int paletteConversion))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvertFromDIBSep
// Purpose:    Same as iplConvertFromDIB, but uses separate
//             parameters for DIB header and data. 
// Returns:    IPLStatus
// Parameters: 
//      dibHeader - The input DIB image header.
//      dibData   - The input DIB image data.
//      image     - The IPL image header with specified attributes.
//                  If the data pointer is NULL, image data memory
//                  will be allocated and the pointer set to it.
//
// Notes:      
*/

IPLAPI ( IPLStatus, iplConvertFromDIBSep, 
         (BITMAPINFOHEADER* dibHeader, const char* dibData, IplImage* image) )


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvertToDIBSep
// Purpose:    Same as iplConvertToDIB, but uses separate
//             parameters for DIB header and data.
// Returns:    
// Parameters: 
//      image     - The input IPL image.
//      dibHeader - The output DIB image header.
//      dibData   - The output DIB image data.
//      dither - 
//          The dithering algorithm to use if applicable.
//          Dithering will be done if  the bit depth in the DIB
//          is less than that of the IPL image.
//          The following algorithms are supported for all dither 
//          type (see iplReduceBits).
//      paletteConversion -
//          Applicable when the DIB is a palette image.
//          Specifies the palette algorithm to use when converting
//          the IPL absolute color image.
//          The following options are supported:
//              IPL_PALCONV_NONE -
//                  The existing palette in the DIB is used.
//              IPL_PALCONV_POPULATE -
//                  The popularity palette conversion algorithm is used.
//              IPL_PALCONV_MEDCUT -
//                  The median cut algorithm palette conversion
//                  algorithm is used.
//
// Notes:      
*/

IPLAPI ( IPLStatus, iplConvertToDIBSep,
         (IplImage* image, BITMAPINFOHEADER* dibHeader, char* dibData, 
         int dither, int paletteConversion) )


/*==========================================================================
      Section: Conversion and Data Exchange Functions
 ==========================================================================*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCopy
// Purpose:    Copies image data from one image to another.
// Returns:    
// Parameters: 
//      srcImage - The source image.
//      dstImage - The resultant image.
//
// Notes:      
*/
 
IPLAPI(void, iplCopy, (IplImage* srcImage, IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplExchange
// Purpose:    Exchanges image data between two images.
// Returns:    
// Parameters: 
//      ImageA - The first image.
//      ImageB - The second image.
//
// Notes:      
*/

IPLAPI(void, iplExchange, (IplImage* ImageA, IplImage* ImageB))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSet, iplSetFP
// Purpose:    Sets a value for an image�s pixel data.
// Returns:    
// Parameters: 
//      image     - An image header with allocated image data.
//      fillValue - The value to set the pixel data.
//
// Notes:      
*/

IPLAPI(void, iplSet, (IplImage* image, int fillValue))
IPLAPI(void, iplSetFP, (IplImage* image, float fillValue))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplPutPixel, iplGetPixel
// Purpose:    Sets/retrieves a value of an image�s pixel.
// Returns:    
// Parameters: 
//      img   - image in IPL-format.
//      x,y   - indexis of the requested pixel.
//      pixel - pointer for values of pixel
//
// Notes:      
//  ignored:
//               img->colorModel     
//               img->channelSeq
//               img->roi
//               img->maskROI
*/

IPLAPI(void, iplPutPixel,(IplImage* img, int x, int y, void* pixel))
IPLAPI(void, iplGetPixel,(IplImage* img, int x, int y, void* pixel))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvert
// Purpose:    Converts source image data to resultant image according to
//             the image headers.
// Returns:    
// Parameters: 
//      srcImage - The source image.
//      dstImage - The resultant image.
//
// Notes:      
*/

IPLAPI(void, iplConvert, (IplImage *srcImage, IplImage *dstImage))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplScale
// Purpose:    
//          1) dst = a + b * src; 
//             a = type_min_dst - b * type_min_src; 
//             b = (type_max_dst - type_min_dst) / (type_max_src - type_min_src).
//
//          2) src(8s,8u,16s,16u,32s) ==> dst(8s,8u,16s,16u,32s); 
//             [type_src_min...type_src_max] ==> [type_dst_min...type_dst_max];
//             src_depth != dst_depth.
// Returns:    
// Parameters: 
//      srcImage        The source image.
//      dstImage        The resultant image.
//
// Notes:      
*/
IPLAPI(IPLStatus, iplScale, (const IplImage* srcImage, IplImage* dstImage ))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplScaleFP
// Purpose:    
//         1)  dst = a + b*src; 
//             a = min_dst - b*min_src; 
//             b = (max_dst - min_dst) / (max_src - min_src). 
//
//         2)  src(32f) ==> dst(8s,8u,16s,16u,32s) + saturation; 
//                 [minVal...maxVal] ==> [type_dst_min...type_dst_max].
//
//         3)  src(8s,8u,16s,16u,32s) ==> dst(32f); 
//                 [type_src_min...type_src_max] ==> [minVal...maxVal].
//
//         4)  src_depth != dst_depth.
// Returns:    
// Parameters: 
//      srcImage                The source image.
//      dstImage                The resultant image.
//      [minVal...maxVal]       Range for depth 32f.
//
// Notes:      
*/

IPLAPI(IPLStatus, iplScaleFP, (const IplImage * srcImage,IplImage * dstImage,
                                                  float  minVal, float  maxVal))


/*==========================================================================
      Section: Arithmetic Functions
 ==========================================================================*/

/*-------------------------  Monadic Operations  --------------------------*/


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAddS, iplAddSFP
// Purpose:    Adds a constant to pixel values of the source image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - The value to increase the pixel values by.
//
// Notes:      
*/

IPLAPI(void, iplAddS,(IplImage* srcImage, IplImage* dstImage, int value))
IPLAPI(void, iplAddSFP,(IplImage* srcImage, IplImage* dstImage, float value))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSubtractS, iplSubtractSFP
// Purpose:    Subtracts a constant from pixel values, or pixel values 
//             from a constant.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - The value to decrease the pixel values by.
//  flip     - A boolean that is used to change the order of subtraction.
//             If false the result pixel value is computed as:
//                 result = pixel_value - value;
//             If true, the result pixel value is computed as
//                 result = value - pixel_value,
//             where pixel_value is the input pixel value.
//
// Notes:      
*/

IPLAPI(void, iplSubtractS,(IplImage* srcImage, IplImage* dstImage, int value,
                                                                  BOOL flip))
IPLAPI(void,iplSubtractSFP,(IplImage* srcImage,IplImage* dstImage,float value,
                                                                  BOOL flip))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMultiplyS, iplMultiplySFP
// Purpose:    Multiplies pixel values by a constant.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - An integer value by which to multiply the pixel values.
//
// Notes:      
*/

IPLAPI(void, iplMultiplyS,(IplImage* srcImage, IplImage* dstImage, int value))
IPLAPI(void, iplMultiplySFP,(IplImage* srcImage, IplImage* dstImage,
                                                                 float value))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMultiplySScale
// Purpose:    Multiplies pixel values by a constant and scales the products.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - A positive value to multiply the pixel values with
//                dstImage  = (srcImage  * value) / VAL_MAX
//
// Notes:      value becomes 0 <= value <= VAL_MAX
*/

IPLAPI(void, iplMultiplySScale,(IplImage* srcImage, IplImage* dstImage,
                                int value))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAbs
// Purpose:    Computes absolute pixel values of the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//
// Notes:      
*/

IPLAPI(void, iplAbs,(IplImage* srcImage, IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSquare
// Purpose:    Squares the pixel values of the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//
// Notes:      
*/

IPLAPI(void, iplSquare,(IplImage* srcImage, IplImage* dstImage))



/*--------------------------  Dyadic Operations  --------------------------*/ 


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAdd
// Purpose:    Combines corresponding pixels of two images by addition.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The resultant image obtained as:
//                  dstImage  = srcImageA  + srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplAdd,(IplImage* srcImageA, IplImage* srcImageB,
                             IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSubtract
// Purpose:    Combines corresponding pixels of two images by subtraction.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The resultant image obtained as:
//                  dstImage  = srcImageA  - srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplSubtract,(IplImage* srcImageA, IplImage* srcImageB,
                          IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMultiply
// Purpose:    Combines corresponding pixels of two images by multiplication.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The resultant image obtained as:
//                  dstImage  = srcImageA  * srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplMultiply,(IplImage* srcImageA, IplImage* srcImageB,
                             IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMultiplyScale
// Purpose:    Multiplies pixel values of two images and scales the products.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The resultant image obtained as:
//                  dstImage  = (srcImageA  * srcImageB) / VAL_MAX
//
// Notes:      The function is implemented only for 8-bit and 16-bit 
//             unsigned data types.
*/

IPLAPI(void, iplMultiplyScale,(IplImage* srcImageA, IplImage* srcImageB,
                               IplImage* dstImage))



/*==========================================================================
      Section: Logical Functions
 ==========================================================================*/

/*-------------------------  Monadic Operations  --------------------------*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplNot
// Purpose:    Performs a bitwise NOT operation on each pixel.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//
// Notes:      
*/

IPLAPI(void, iplNot,(IplImage* srcImage, IplImage* dstImage))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplLShiftS
// Purpose:    Shifts pixel values� bits to the left.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nShift   - The number of bits to shift each pixel value to the left to.
//
// Notes:      
*/

IPLAPI(void, iplLShiftS,(IplImage* srcImage, IplImage* dstImage,
                             unsigned int nShift))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplRShiftS
// Purpose:    Shifts pixel values� bits to the right.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nShift   - The number of bits to shift each pixel value to the right to.
//
// Notes:      
*/

IPLAPI(void, iplRShiftS,(IplImage* srcImage, IplImage* dstImage,
                             unsigned int nShift))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAndS
// Purpose:    Performs a bitwise AND operation of each pixel with a constant.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - The bit sequence used to perform the bitwise operation
//             on each pixel. Only the number of bits corresponding to the 
//             data type of the image are used.
//
// Notes:      
*/

IPLAPI(void, iplAndS,(IplImage* srcImage, IplImage* dstImage,
                             unsigned int value))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplOrS
// Purpose:    Performs a bitwise OR operation of each pixel with a constant.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - The bit sequence used to perform the bitwise operation
//             on each pixel. Only the number of bits corresponding to the 
//             data type of the image are used.
//
// Notes:      
*/

IPLAPI(void, iplOrS,(IplImage* srcImage, IplImage* dstImage,
                             unsigned int value))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplXorS
// Purpose:    Performs a bitwise XOR operation of each pixel with a constant.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  value    - The bit sequence used to perform the bitwise operation
//             on each pixel. Only the number of bits corresponding to the 
//             data type of the image are used.
//
// Notes:      
*/

IPLAPI(void, iplXorS,(IplImage* srcImage, IplImage* dstImage,
                             unsigned int value))



/*--------------------------  Dyadic Operations  --------------------------*/ 


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAnd
// Purpose:    Combines corresponding pixels of two images by a bitwise AND
//             operation.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The image resulting from the bitwise operation 
//              between input images srcImageA and srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplAnd,(IplImage* srcImageA, IplImage* srcImageB,
                             IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplOr
// Purpose:    Combines corresponding pixels of two images by a 
//             bitwise OR operation.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The image resulting from the bitwise operation 
//              between input images srcImageA and srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplOr,(IplImage* srcImageA, IplImage* srcImageB,
                             IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplXor
// Purpose:    Combines corresponding pixels of two images by a 
//             bitwise XOR operation.
// Returns:    
// Parameters: 
//  srcImageA - The first source image.
//  srcImageB - The second source image.
//  dstImage  - The image resulting from the bitwise operation 
//              between input images srcImageA and srcImageB.
//
// Notes:      
*/

IPLAPI(void, iplXor,(IplImage* srcImageA, IplImage* srcImageB,
                             IplImage* dstImage))



/*==========================================================================
      Section: Alpha-blending Functions
 ==========================================================================*/

#define IPL_COMPOSITE_OVER    0
#define IPL_COMPOSITE_IN      1
#define IPL_COMPOSITE_OUT     2
#define IPL_COMPOSITE_ATOP    3
#define IPL_COMPOSITE_XOR     4
#define IPL_COMPOSITE_PLUS    5


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplAlphaComposite, iplAlphaCompositeC
// Purpose:    Composite two images using alpha (opacity) values.
// Returns:    
// Parameters: 
//  srcImageA     - The foreground image.
//  srcImageB     - The background image.
//  dstImage      - The resultant image.
//  compositeType - The type of composition to perform. 
//  aA            - The constant alpha value to use for the source 
//                  image srcImageA. Should be a positive number.
//  aB            - The constant alpha value to use for the source 
//                  image srcImageB. Should be a positive number.
//  alphaImageA   - The image to use as the  alpha channel for srcImageA.
//
//  alphaImageB   - The image to use as the  alpha channel for srcImageB.
//  alphaImageDst - The image to use as the  alpha channel for dstImage.
//  premulAlpha   - Indicates that the input images contain
//                  premultiplied alpha values.
//  divideMode    - Normally set to false. When true, the result pixel color
//                  (in table "Image Compositing Operations") is further 
//                  divided by the result alpha value to get the final result
//                  pixel color.
//
// Notes:      value becomes aA <= VAL_MAX, aB <= VAL_MAX
*/

IPLAPI(void, iplAlphaComposite,(IplImage* srcImageA, IplImage* srcImageB,
            IplImage* dstImage, int compositeType, IplImage* alphaImageA, 
            IplImage* alphaImageB, IplImage* alphaImageDst,
            BOOL premulAlpha, BOOL divideMode))
IPLAPI(void, iplAlphaCompositeC,(IplImage* srcImageA, IplImage* srcImageB,
            IplImage* dstImage, int compositeType, int aA, int aB,
            BOOL premulALpha, BOOL divideMode))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplPreMultiplyAlpha
// Purpose:    Pre-multiplies alpha values of an image.
// Returns:    
// Parameters: 
//  image      - The image for which the alpha premultiplication is done.
//  alphaValue - The global alpha value to use in the range 0 to VAL_MAX.
//               If this value is negative (e.g. -1) the internal alpha channel
//               of the image is used (it is an error if an alpha channel does
//               not exist).
//
// Notes:      value becomes alphaValue <= VAL_MAX
*/

IPLAPI(void, iplPreMultiplyAlpha, (IplImage* image, int alphaValue))


/*==========================================================================
      Section: Filtering Functions
 ==========================================================================*/

/*------------------------  Management of kernels  ------------------------*/


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateConvKernel, iplCreateConvKernelFP, 
//             iplCreateConvKernelChar
// Purpose:    Creates a convolution kernel.
// Returns:    A pointer to the convolution kernel structure IplConvKernel.
// Parameters: 
//  nCols    - The number of columns in the convolution kernel.
//  nRows    - The number of rows in the convolution kernel.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the kernel.
//  nShiftR  - The number of  bits to shift (to the right) the resulting
//             output pixel of each convolution.
//
// Notes:      iplCreateConvKernelChar used only to convert from IPL1.1 to 
//             IPL2.0. It just convert the char kernel values into integer 
//             which now used in IPL2.0 to create kernel structures.
*/

IPLAPI(IplConvKernel*, iplCreateConvKernel,(int nCols, int nRows,
                       int anchorX, int anchorY, int* values, int nShiftR))
IPLAPI(IplConvKernelFP*, iplCreateConvKernelFP,(int nCols, int nRows,
                       int anchorX, int anchorY, float* values))
IPLAPI(IplConvKernel*, iplCreateConvKernelChar,(int nCols, int nRows,
                       int anchorX, int anchorY, char* values, int nShiftR))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplGetConvKernel, iplGetConvKernelFP, iplGetConvKernelChar
// Purpose:    Reads the attributes of a convolution kernel.
// Returns:    
// Parameters: 
//  kernel   - The kernel to get the attributes for.
//             The attributes are returned in the remaining arguments.
//  nCols    - A pointer to the the number of columns in the convolution 
//             kernel. Set by the function.
//  nRows    - A pointer to the number of rows in the convolution kernel.
//             Set by the function.
//  anchorX,
//  anchorY  - Pointers to the [x,y] coordinates of the anchor cell
//             in the kernel.
//  nShiftR  - A pointer to the number of  bits to shift (to the right)
//             the resulting output pixel of each convolution.
//             Set by the function.
//
// Notes:      iplGetConvKernelChar used only to convert from IPL1.0 to IPL2.0.
//             It just convert the integer kernel values into char which 
//             earlier used in IPL1.1 while getting kernel values.
//             Function works correctly only if kernel values lies 
//             in interval [-256,255].
*/

IPLAPI(void,iplGetConvKernel,(IplConvKernel* kernel, int* nCols, int* nRows,
                      int* anchorX, int* anchorY, int** values, int *nShiftR))
IPLAPI(void,iplGetConvKernelFP,(IplConvKernelFP* kernel,int* nCols, int* nRows,
                      int* anchorX, int* anchorY, float** values))
IPLAPI(void,iplGetConvKernelChar,(IplConvKernel* kernel, int* nCols, int* nRows,
                      int* anchorX, int* anchorY, char** values, int *nShiftR))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeleteConvKernel, iplDeleteConvKernelFP
// Purpose:    Deletes a convolution kernel.
// Returns:    
// Parameters: 
//  kernel   - The kernel to delete.
//
// Notes:      
*/

IPLAPI(void, iplDeleteConvKernel,(IplConvKernel* kernel))
IPLAPI(void, iplDeleteConvKernelFP,(IplConvKernelFP* kernel))



/*---------------------------  Linear Filters  ----------------------------*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplBlur
// Purpose:    Applies simple neighborhood averaging filter to blur the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nCols    - Number of columns in the neighbourhood to use.
//  nRows    - Number of rows in the neighbourhood to use.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the neighbourhood.
//
// Notes:      
*/

IPLAPI(void, iplBlur,(IplImage* srcImage, IplImage* dstImage, int nCols, 
       int nRows, int anchorX, int anchorY))


#define IPL_SUM               0
#define IPL_SUMSQ             1
#define IPL_SUMSQROOT         2
#define IPL_MAX               3
#define IPL_MIN               4


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvolve2D, iplConvolve2DFP
// Purpose:    Convolves an image with one or more convolution kernels.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  kernel   - A pointer to an array of pointers to convolution kernels.
//             The length of the array is nKernels. Can be one or more kernels.
//  nKernels - The number of kernels in the array kernel.
//  combineMethod -
//             The way in which the results of applying each kernel
//             should be combined. This argument is ignored
//             when a single kernel is used. The following combinations
//             are supported:
//                IPL_SUM         Sums the results.
//                IPL_SUMSQ       Sums the squares of the  results.
//                IPL_SUMSQROOT   Sums the squares of the results
//                                and then takes the sqaure root.
//                IPL_MAX         Takes the maximum of the results.
//                IPL_MIN         Takes the minimum of the results.
//
// Notes:      
*/

IPLAPI(void, iplConvolve2D,(IplImage* srcImage, IplImage* dstImage,
                IplConvKernel** kernel, int nKernels, int combineMethod))
IPLAPI(void, iplConvolve2DFP,(IplImage* srcImage, IplImage* dstImage,
                IplConvKernelFP** kernel, int nKernels, int combineMethod))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplFixedFilter
// Purpose:    Convolves an image with a predefined kernel.
// Returns:    zero if the execution is completed successfully, 
//             and a non-zero integer if an error occurred.
// Parameters: IPLStatus
//  srcImage - The source image.
//  dstImage - The resultant image.
//  filter   - One of the predefined kernels:
//               IPL_PREWITT_3x3_V=0
//               IPL_PREWITT_3x3_H
//               IPL_SOBEL_3x3_V
//               IPL_SOBEL_3x3_H
//               IPL_LAPLACIAN_3x3
//               IPL_LAPLACIAN_5x5
//               IPL_GAUSSIAN_3x3
//               IPL_GAUSSIAN_5x5
//               IPL_HIPASS_3x3
//               IPL_HIPASS_5x5
//               IPL_SHARPEN_3x3
//
// Notes:      
*/

IPLAPI(IPLStatus, iplFixedFilter,(IplImage* srcImage, IplImage* dstImage,
                                  IplFilter filter))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplConvolveSep2D, iplConvolveSep2DFP
// Purpose:    Convolves an image with a separable convolution kernel.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  xKernel  - The x or row kernel. Should contain only one row.
//  ykernel  - The y or column kernel. Should contain only one column.
//
// Notes:      
*/

IPLAPI(void, iplConvolveSep2D,(IplImage* srcImage, IplImage* dstImage,
                      IplConvKernel* xKernel, IplConvKernel* yKernel))
IPLAPI(void, iplConvolveSep2DFP,(IplImage* srcImage,
                          IplImage* dstImage,
                          IplConvKernelFP* xKernel,
                          IplConvKernelFP* yKernel))


/*-------------------------  Non Linear Filters  --------------------------*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMedianFilter
// Purpose:    Apply a median filter to the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nCols    - Number of columns in the neighbourhood to use.
//  nRows    - Number of rows in the neighbourhood to use.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the neighbourhood.
//
// Notes:      
*/

IPLAPI(void, iplMedianFilter,(IplImage* srcImage, IplImage* dstImage,
                     int nCols, int nRows, int anchorX, int anchorY))

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplColorMedianFilter
// Purpose:    Apply a color median filter to the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nCols    - Number of columns in the neighbourhood to use.
//  nRows    - Number of rows in the neighbourhood to use.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the neighbourhood.
//
// Notes:      
*/

IPLAPI(void, iplColorMedianFilter,(IplImage* srcImage, IplImage* dstImage,
                     int nCols, int nRows, int anchorX, int anchorY))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMaxFilter
// Purpose:    Apply a max filter to the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nCols    - Number of columns in the neighbourhood to use.
//  nRows    - Number of rows in the neighbourhood to use.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the neighbourhood.
//
// Notes:      
*/

IPLAPI(void, iplMaxFilter,(IplImage* srcImage, IplImage* dstImage,
                  int nCols, int nRows, int anchorX, int anchorY))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMinFilter
// Purpose:    Apply a min filter to the image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  nCols    - Number of columns in the neighbourhood to use.
//  nRows    - Number of rows in the neighbourhood to use.
//  anchorX,
//  anchorY  - The [x,y] coordinates of the anchor cell in the neighbourhood.
//
// Notes:      
*/

IPLAPI(void, iplMinFilter,(IplImage* srcImage, IplImage* dstImage, int nCols,
                          int nRows, int anchorX, int anchorY))



/*==========================================================================
      Section: Fast Fourier and Discrete Cosine Transforms
 ==========================================================================*/

/*-----------------------  Fast Fourier Transform  ------------------------*/

#define  IPL_FFT_Forw                     1
#define  IPL_FFT_Inv                      2
#define  IPL_FFT_NoScale                  4
#define  IPL_FFT_UseInt                  16
#define  IPL_FFT_UseFloat                32
#define  IPL_FFT_Free                   128


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplRealFft2D
// Purpose:    Computes the forward or inverse 2D FFT of an image.
// Returns:    
// Parameters: 
//  srcImage - The source image. Any mask ROI specified will be ignored.
//  dstImage - The resultant image in CcsPerm2D format containing
//             the Fourier coefficients. This image cannot be the same
//             as the input image srcImage Any mask ROI specified will be
//             ignored. This image should be a multi-channel image containing
//             as many channels as srcImage. The data type for the image
//             should be 8, 16 or 32 bits.
//  flags    - Specifies how the FFT should be performed.
//             Integer number in which every bit have next specifications:
//                IPL_FFT_Forw       - forward transform,
//                IPL_FFT_Inv        - inverse transform,
//                IPL_FFT_NoScale    - in inverse transform absent to scale,
//                IPL_FFT_UseInt     - use only int   core,
//                IPL_FFT_UseFloat   - use only float core,
//                IPL_FFT_Free       - only free all working arrays and exit.
//
// Notes:      
*/

IPLAPI(void, iplRealFft2D,(IplImage* srcImage, IplImage* dstImage, int flags))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCcsFft2D
// Purpose:    Computes the forward or inverse 2D FFT of an image 
//             in complex-conjugate format.
// Returns:    
// Parameters: 
//  srcImage - The source image in CcsPerm2D format.
//             Any mask ROI specified will be ignored.
//  dstImage - The resultant image. This image cannot be the same as
//             the input image srcImage.  
//             Any mask ROI specified will be ignored.
//             This image should be a multi-channel image containing as many
//             channels as srcImage.
//  flags    - Specifies how the FFT should be performed.
//             Integer number in which every bit have next specifications:
//                IPL_FFT_Forw       - forward transform,
//                IPL_FFT_Inv        - inverse transform,
//                IPL_FFT_NoScale    - in inverse transform absent to scale,
//                IPL_FFT_UseInt     - use only int   core,
//                IPL_FFT_UseFloat   - use only float core,
//                IPL_FFT_Free       - only free all working arrays and exit.
//
// Notes:      
*/

IPLAPI(void, iplCcsFft2D,(IplImage* srcImage, IplImage* dstImage, int flags))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMpyRCPack2D
// Purpose:    Myltiply two IPL-images in "RCPack2D" format
//             and put result into destination image in "RCPack2D" format,
//             for 8S, 16S, 32S, 32F depth, 
//             without ROI, and with ROI,
//             without maskROI, for not tiled images
// Returns:    None
// Parameters:
//     srcA   - First  source image in "RCPack2D" format.
//     srcB   - Second sourse image in "RCPack2D" format.
//     dst    - Destination image   in "RCPack2D" format.
//
// Notes:      
*/

IPLAPI(void, iplMpyRCPack2D,(IplImage* srcA, IplImage* srcB, IplImage* dst))



/*----------------------  Discrete Cosine Transform  ----------------------*/

#define  IPL_DCT_Forward      1
#define  IPL_DCT_Inverse      2
#define  IPL_DCT_Free         8
#define  IPL_DCT_UseInpBuf    16


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDCT2D
// Purpose:    Computes the forward or inverse 2D DCT of an image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image containing the DCT
//             coefficients. This image must be a multi-channel
//             image containing the same number of channels
//             as srcImage. The data type for the image must
//             be 8, 16 or 32 bits.
//             This image cannot be the same as the input
//             image srcImage (that is, an in-place operation is
//             not allowed).
//  flags    - Specifies how to perform the DCT. This is an
//             integer whose bits can be assigned the following
//             values using bitwise logical OR:
//                  IPL_DCT_Forward   - Do forward transform.
//                  IPL_DCT_Inverse   - Do inverse transform.
//                  IPL_DCT_Free      - Only free all working arrays and exit.
//                  IPL_DCT_UseInpBuf - Use the input image array for the 
//                                      intermediate calculations. 
//                                      The performance of DCT increases, but
//                                      the input image is destroyed. You may 
//                                      use this value only if both the source
//                                      and destination image data types are 
//                                      16-bit signed.
//
// Notes:      
*/

IPLAPI(void,iplDCT2D,(IplImage* src, IplImage* dst, int flags))



/*==========================================================================
      Section: Morphological Operations
 ==========================================================================*/


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplErode
// Purpose:    Erodes the image.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  nIterations - The number of times to erode the image.
//
// Notes:      
*/

IPLAPI(void, iplErode,(IplImage* srcImage, IplImage* dstImage,
                             int nIterations))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDilate
// Purpose:    Dilates the image.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  nIterations - The number of times to dilate the image.
//
// Notes:      
*/

IPLAPI(void, iplDilate,(IplImage* srcImage, IplImage* dstImage,
                             int nIterations))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplOpen
// Purpose:    Opens the image by performing erosions followed by dilations.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  nIterations - The number of times to erode and dilate the image.
//
// Notes:      
*/

IPLAPI(void, iplOpen,(IplImage* srcImage, IplImage* dstImage,
                             int nIterations))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplClose
// Purpose:    Closes the image by performing dilations followed by erosions.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  nIterations - The number of times to dilate and erode the image.
//
// Notes:      
*/

IPLAPI(void, iplClose,(IplImage* srcImage, IplImage* dstImage,
                             int nIterations))


/*==========================================================================
      Section: Color Space Conversion
 ==========================================================================*/

#define IPL_JITTER_NONE       0
#define IPL_DITHER_NONE       1
#define IPL_DITHER_FS         2
#define IPL_DITHER_JJH        4
#define IPL_DITHER_STUCKEY    8
#define IPL_DITHER_BAYER     16


/* /////////////////////////////////////////////////////////////////////////
//  Name:       iplReduceBits
//  Purpose:    Transforms Image of a higher bit resolution to a lower bit resolution ,or
//              halftoning to multiple output levels.
//  Arguments:  
//  srcImage  - The source image.
//  dstImage  - The resultant image.
//     noise  - the number specifying the noise added,is set in percentage of a range [0..100] 
//     levels - the number of output levels for halftoning (dithering)[2.. MAX_LEVELS - 1],
//            where  MAX_LEVELS is  0x01 << depth and depth is depth of the destination image
//     ditherType  -  the type of dithering to be used. The following are allowed
//        IPL_DITHER_NONE     no dithering is done
//        IPL_DITHER_STUCKEY  Stuckey's dithering algorithm
//        IPL_DITHER_FS       Floid-Steinberg's dithering algorithm
//        IPL_DITHER_JJH      Jarvice-Judice-Hinke's dithering algorithm
//        IPL_DITHER_BAYER    Bayer's dithering algorithm
//  32 bit per channel  -> 32,16, 8, 1  bit per channel
//  16 bit per channel  -> 16, 8, 1     bit per channel
//  8  bit per channel  ->  8, 1        bit per channel( 1u for Gray only)
//  Notes: Algorithm taken from , ," 
//         "Graphics Gems, Vol.1,1990.
//   reducing uses the equation dst = src*(((1<<n) -1)/((1<<m) - 1)),
//  where m-source bit depth, n - destination bit depth.
*/

IPLAPI(void, iplReduceBits,(IplImage* srcImage, IplImage* dstImage,
                            int noise, int ditherType, int levels))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplBitonalToGray
// Purpose:    Converts a bitonal image to gray scale.
// Returns:    
// Parameters: 
//  srcImage  - The source image.
//  dstImage  - The resultant image.
//  ZeroScale - The value that zero pixels of the source image
//              should have in the resultant image.
//  OneScale  - The value given to a resultant pixel if the
//              corresponding input pixel is 1.
//
// Notes:      
*/

IPLAPI(void, iplBitonalToGray, (IplImage *srcImage,IplImage *dstImage,
                                int ZeroScale, int OneScale))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplColorToGray
// Purpose:    Converts a color image to gray scale.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//
// Notes:      
*/

IPLAPI(void, iplColorToGray,(IplImage* srcImage, IplImage* dstImage))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplGrayToColor
// Purpose:    Converts a gray scale to color image (pseudo color).
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  FractR,
//  FractG,
//  FractB   - The red, green and blue intensities for image
//             reconstruction.
//
// Notes:      
*/

IPLAPI(void, iplGrayToColor, (IplImage* srcImage, IplImage* dstImage,
                              float FractR, float FractG, float FractB))


                              
/*------------------  Color model conversion functions  -------------------*/

/* /////////////////////////////////////////////////////////////////////////
// Purpose:  Converts from one color model to other
//
// Notes:    See documentation for information about color models,
//   and also, the iplRGB2YUV and iplYUV2RGB functions have the special features: 
//   - a destination image of the iplRGB2YUV function can be of the plane order if a 
//   source image is of the pixel order and both are of the 8u depth;
//   - a destination image of the iplYUV2RGB2 function can be of the pixel order if a 
//   source image is of the plane order and both are of the 8u depth;
*/

IPLAPI(void, iplRGB2HSV,(IplImage* rgbImage, IplImage* hsvImage))
IPLAPI(void, iplHSV2RGB,(IplImage* hsvImage, IplImage* rgbImage))

IPLAPI(void, iplRGB2HLS,(IplImage* rgbImage, IplImage* hlsImage))
IPLAPI(void, iplHLS2RGB,(IplImage* hlsImage, IplImage* rgbImage))

IPLAPI(void, iplRGB2XYZ,(IplImage* srcImage, IplImage* dstImage))
IPLAPI(void, iplXYZ2RGB,(IplImage* srcImage, IplImage* dstImage))

IPLAPI(void, iplRGB2LUV,(IplImage* rgbImage, IplImage* LUVImage))
IPLAPI(void, iplLUV2RGB,(IplImage* LUVImage, IplImage* rgbImage))

IPLAPI(void, iplRGB2YUV,(IplImage* rgbImage, IplImage* YUVImage))
IPLAPI(void, iplYUV2RGB,(IplImage* YUVImage, IplImage* rgbImage))

IPLAPI(void, iplRGB2YCrCb,(IplImage* rgbImage, IplImage* YCrCbImage))
IPLAPI(void, iplYCrCb2RGB,(IplImage* YCrCbImage, IplImage* rgbImage))

IPLAPI(void, iplYCC2RGB,(IplImage* YCCImage, IplImage* rgbImage))


/*------------------------  Color Twist Matrices  -------------------------*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplCreateColorTwist
// Purpose:    Creates a color-twist matrix data structure.
// Returns:    A pointer to the iplColorTwist data structure containing
//             the color twist matrix in form suitable for efficient 
//             computation. This structure can then be used in the function
//             iplApplyColorTwist().
// Parameters: 
//  data         - An array containing the sixteen values that constitute
//                 the color twist matrix. The values are in row wise order.
//                 Color twist values that lie in the range -1 to 1 should be
//                 scaled up to lie in the range -231 to 231
//                 (Simply multiply the floating point number in the -1 to 1
//                 range by 231)
//  scalingValue - The scaling value - the exponent of a power of 2 -
//                 that was used (e.g. 31 if 231  was used to multiply 
//                 the values) to convert to integer values. Will be used 
//                 for normalization.
//
// Notes:      
*/

IPLAPI(IplColorTwist*, iplCreateColorTwist, (int data[16], int scalingValue))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplSetColorTwist
// Purpose:    Sets a color-twist matrix data structure.
// Returns:    
// Parameters: 
//  data         - An array containing the sixteen values that constitute
//                 the color twist matrix. The values are in row wise order.
//                 Color twist values that lie in the range -1 to 1 should be
//                 scaled up to lie in the range -231 to 231
//                 (Simply multiply the floating point number in the -1 to 1
//                 range by 231)
//  scalingValue - The scaling value - the exponent of a power of 2 -
//                 that was used (e.g. 31 if 231  was used to multiply the 
//                 values) to convert to integer values. Will be used for 
//                 normalization.
//  cTwist       - The color  twist matrix data structure that was created
//                 by a call to the function iplCreateColorTwist().
//
// Notes:      
*/

IPLAPI(void, iplSetColorTwist, (IplColorTwist *cTwist,int data[16],
                                                            int scalingValue))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDeleteColorTwist
// Purpose:    Frees memory used for a color-twist matrix.
// Returns:    
// Parameters: 
//    cTwist - The color  twist matrix data structure that was created
//             by a call to the function iplCreateColorTwist().
//
// Notes:      
*/

IPLAPI (void, iplDeleteColorTwist, (IplColorTwist *cTwist))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplApplyColorTwist
// Purpose:    Applies a color-twist matrix to an image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  cTwist   - The color  twist matrix data structure that was prepared
//             by a call to the function iplSetColorTwist().
//  offset   - An offset value that will be added to each pixel channel
//             after multiplication with the color twist matrix.
//
// Notes:      
*/

IPLAPI(void, iplApplyColorTwist, (IplImage* srcImage, IplImage* dstImage,
                        IplColorTwist* cTwist, int offset))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplColorTwistFP
// Purpose:    Applies a color-twist matrix to an image.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  TwistFP  - The matrix.
//
// Notes:      
//             |Y11|   |T11 T12 T13|   |X11|   |T14|
//             |Y21| = |T21 T22 T23| * |X21| + |T24|
//             |Y31|   |T31 T32 T33|   |X31|   |T34|
//
//             TwistFP[12] = {T11,T12,T13,T14,T21,T22,T23,T24,T31,T32,T33,T34};
//
//             X11(Y11) - The first channel of the srcImage(dstImage);
//             X21(Y21) - The second channel of the srcImage(dstImage);
//             X31(Y31) - The third channel of the srcImage(dstImage);
//
//             srcImage->depth == dstImage->depth == 32F.
*/

IPLAPI(IPLStatus, iplColorTwistFP,(const IplImage* srcImage, IplImage* dstImage, 
                                    float * TwistFP))

/*==========================================================================
      Section: Histogram and Thresholding Functions
 ==========================================================================*/


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplThreshold
// Purpose:    Performs a simple thresholding of an image.
// Returns:    
// Parameters: 
//  srcImage  - The source image.
//  dstImage  - The resultant image.
//  threshold - The threshold value to use for each pixel.
//              The pixel value in the output is set to the maximum
//              representable value if it is greater than or equal to the
//              threshold value (for each channel). Otherwise the pixel value
//              in the output is set to minimum representable value.
//
// Notes:      
*/

IPLAPI(void, iplThreshold, (IplImage* srcImage, IplImage* dstImage,
                                                               int threshold))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplContrastStretch
// Purpose:    Stretches the contrast of an image using an intensity 
//             transformation.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  lut      - An array of pointers to LUTs - one for each channel.
//             Each lookup table should have the key and value arrays
//             fully initialized. One or more channels may share the same LUT.
//             Specifies an intensity transformation.
//
// Notes:      
*/

IPLAPI(void, iplContrastStretch,(IplImage* srcImage, IplImage* dstImage,
                             IplLUT** lut))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplComputeHisto
// Purpose:    Computes the intensity histogram of an image.
// Returns:    
// Parameters: 
//  srcImage - The source image for which the Histogram will be computed.
//  lut      - An array of pointers to LUTs - one for each channel.
//             Each lookup table should have  the key array fully initialized.
//             The value array will be filled by this function.
//             The same LUT can be shared by one or more channels.
//
// Notes:      
*/

IPLAPI(void, iplComputeHisto, (IplImage* srcImage, IplLUT** lut))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplHistoEqualize
// Purpose:    Enhances an image by flattening its intensity histogram.
// Returns:    
// Parameters: 
//  srcImage - The source image for which the Histogram will be computed.
//  dstImage - The resultant image after equalization.
//  lut      - The Histogram of the image represented as an array of pointers
//             to LUTs - one for each channel. Each  lookup table should have
//             the key and value arrays fully initialized.
//             This LUTs will contain flattened Histograms after this function
//             is executed.
//
// Notes:      
*/

IPLAPI(void, iplHistoEqualize,(IplImage* srcImage, IplImage* dstImage,
                             IplLUT** lut))



/*==========================================================================
      Section: Geometric Transformation Functions
 ==========================================================================*/

/*------------------------  Kind of Interpolation  ------------------------*/

#define IPL_INTER_NN          0
#define IPL_INTER_LINEAR      1
#define IPL_INTER_CUBIC       2
#define IPL_INTER_SUPER       3
#define IPL_SMOOTH_EDGE      16

/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplWarpAffine
//  Purpose:        makes Affine transform of image.
//                  |X'|   |a11 a12| |X| |a13|
//                  |  | = |       |*| |+|   |
//                  |Y'|   |a21 a22| |Y| |a23|
//  Context:
//  Returns:        None.
//  Parameters:
//      srcImage     The source image.
//      dstImage     The resultant image.
//      coeffs       The transform matrix
//      interpolate  The type of interpolation to perform for resampling
//                   the input image. The following are currently supported.
//                   IPL_INTER_NN     Nearest neighbour interpolation.
//                   IPL_INTER_LINEAR Linear interpolation.
//                   IPL_INTER_CUBIC  Cubic convolution interpolation.
//                   IPL_SMOOTH_EDGE  Smooth edges. Can be added to
//                                    interpolation by using bitwise logical OR.
//  Notes:
*/

IPLAPI(void, iplWarpAffine, (IplImage* srcImage, IplImage* dstImage, 
                             const double coeffs[2][3], int interpolate))

/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplRemap
//  Purpose:        Remap srcImage with map.
//                  dst[i,j] = src[xMap[i], yMap[j]]
//  Returns:        None.
//  Parameters:
//      srcImage         The source image.
//      xMap             The image with x coords of map.
//      yMap             The image with y coords of map.
//      dstImage         The resultant image.
//      interpolate      The type of interpolation to perform for resampling
//                       the input image. The following are currently supported.
//                       IPL_INTER_NN       Nearest neighbour interpolation.
//                       IPL_INTER_LINEAR   Linear interpolation.
//                       IPL_INTER_CUBIC    Cubic convolution interpolation.
//  Notes:
*/

IPLAPI(void, iplRemap, (IplImage* srcImage, IplImage* xMap, IplImage* yMap, 
                                         IplImage* dstImage, int interpolate))



/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplShear
//  Purpose:        Makes shear transform of image.
//                  |X'|   |1    xShear| |X|
//                  |  | = |           |*| |
//                  |Y'|   |yShear   1 | |Y|
//  Returns:        None.
//  Parameters:
//      srcImage     The source image.
//      dstImage     The resultant image.
//      interpolate  The type of interpolation to perform for resampling
//                   the input image. The following are currently supported.
//                   IPL_INTER_NN     Nearest neighbour interpolation.
//                   IPL_INTER_LINEAR Linear interpolation.
//                   IPL_INTER_CUBIC  Cubic convolution interpolation.
//                   IPL_SMOOTH_EDGE  Smooth edges. Can be added to
//                                    interpolation by using bitwise logical OR.
//  Notes:
*/

IPLAPI(void, iplShear, (IplImage* srcImage, IplImage* dstImage,
                        double xShear, double yShear, double xShift, 
                        double yShift, int interpolate))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplRotate
//  Purpose:        rotates image about (0, 0) on angle.
//  Returns:        None.
//  Parameters:
//      srcImage     The source image.
//      dstImage     The resultant image.
//      interpolate  The type of interpolation to perform for resampling
//                   the input image. The following are currently supported.
//                   IPL_INTER_NN     Nearest neighbour interpolation.
//                   IPL_INTER_LINEAR Linear interpolation.
//                   IPL_INTER_CUBIC  Cubic convolution interpolation.
//                   IPL_SMOOTH_EDGE  Smooth edges. Can be added to
//                                    interpolation by using bitwise logical OR.
//  Notes:
*/

IPLAPI(void, iplRotate, (IplImage* srcImage, IplImage* dstImage, double angle,
                         double xShift, double yShift, int interpolate))


/* /////////////////////////////////////////////////////////////////////////
//  Name:        iplGetAffineQuad
//  Purpose:     calculates coordinates of quadrangle from transformed image ROI.
//  Returns:     None.
//  Parameters:
//      image  - The source image.
//      coeffs -    The transform matrix
//                  |X'|   |a11 a12| |X| |a13|
//                  |  | = |       |*| |+|   |
//                  |Y'|   |a21 a22| |Y| |a23|
//      quadr       resultant quadrangle
//  Notes:
*/

IPLAPI(void, iplGetAffineQuad, (IplImage* image, const double coeffs[2][3],
                                                       double quad[4][2]))
IPLAPI(void, iplGetAffineQuadROI, (IplROI* roi, const double coeffs[2][3],
                                                       double quad[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetAffineBound
//  Purpose:        calculates bounding rectangle of the transformed image ROI.
//  Returns:        None.
//  Parameters:
//      image    The source image.
//      coeffs      The transform matrix
//                  |X'|   |a11 a12| |X| |a13|
//                  |  | = |       |*| |+|   |
//                  |Y'|   |a21 a22| |Y| |a23|
//      rect        resultant bounding rectangle
//  Notes:
*/

IPLAPI(void, iplGetAffineBound, (IplImage* image, const double coeffs[2][3],
                                 double rect[2][2]))
IPLAPI(void, iplGetAffineBoundROI, (IplROI* roi, const double coeffs[2][3],
                                 double rect[2][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetAffineTransform
//  Purpose:        calculates transform matrix from vertexes of quadrangle.
//  Returns:        None.
//  Parameters:
//      image    The source image.
//      coeffs      The resultant transform matrix
//                  |X'|   |a11 a12| |X| |a13|
//                  |  | = |       |*| |+|   |
//                  |Y'|   |a21 a22| |Y| |a23|
//      quadr       quadrangle
//  Notes:
*/

IPLAPI(void, iplGetAffineTransform, (IplImage* image, double coeffs[2][3],
                                     const double quad[4][2]))
IPLAPI(void, iplGetAffineTransformROI, (IplROI* roi, double coeffs[2][3],
                                     const double quad[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetRotateShift
//  Purpose:        recalculates shifts for rotation around point (x, y).
//  Returns:        None.
//  Parameters:
//                  xCenter, yCenter    new center of rotation
//                  angle               the angle of rotation
//  Notes:
*F*/

IPLAPI(void, iplGetRotateShift, (double xCenter, double yCenter, double angle,
                              double *xShift, double *yShift))


#define IPL_WARP_R_TO_Q  0
#define IPL_WARP_Q_TO_R  1

/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplWarpBilinear, iplWarpBilinearQ
//  Purpose:        makes bilinear transform of image.
//                  from rectangle to quadrangle use transform
//                  |X|   |a11|      |a12 a13| |J|   |a14|
//                  | | = |   |*JI + |       |*| | + |   |
//                  |Y|   |a21|      |a22 a23| |I|   |a24|
//                  from quadrangle to rectungle use inverse transform
//                  0 = J*J * (-(x1-x0)*(y3-y2)+(x3-x2)*(y1-y0)) +
//                      J * (X*(y3-y2+y1-y0) - Y*(x3-x2+x1-x0) +
//                      ((x0-x3)*(y1-y0)-(x1-x0)*(y0-y3))) +
//                      (X*(y0-y3) - Y*(x0-x3))
//                  0 = I*I * (-(x0-x3)*(y2-y1)+(x2-x1)*(y0-y3)) +
//                      I * (X*(y3-y2+y1-y0) - Y*(x3-x2+x1-x0) +
//                      (-(x0-x3)*(y1-y0)+(y0-y3)*(x1-x0))) +
//                      (-X*(y1-y0) + Y*(x1-x0))
//                 In addition, I & J are bounded by the relation:
//                 0 = I * (-(x0-x3)*(y2-y1)+(x2-x1)*(y0-y3)) +
//                     J * (-(x1-x0)*(y3-y2)+(x3-x2)*(y1-y0)) +
//                    (X * (y3-y2+y1-y0) - Y * (x3-x2+x1-x0))
//                 J=0..1, I=0..1;
//                 J=j/jmax, I=i/imax;
//                 (j,i) - coordinates of a pixel in the square rectangle
//                 X=x-x0, Y=y-y0;
//                 (x,y) - coordinates of the pixel in the qudrangle
//
//  Returns:        None.
//  Parameters:
//      srcImage     The source image.
//      dstImage     The resultant image.
//      quad         The vertexes of quadrangle.
//      warpFlag     If warpFlag is IPL_WARP_R_TO_Q, the transform is from 
//                   rectangle srcImage->roi to quadrangle in dstImage.
//                   If warpFlag is IPL_WARP_Q_TO_R, the transform is from 
//                   quadrangle in srcImage to rectangle dstImage->roi.
//      interpolate  The type of interpolation to perform for resampling
//                   the input image. The following are currently supported:
//                   IPL_INTER_NN     Nearest neighbour interpolation.
//                   IPL_INTER_LINEAR Linear interpolation.
//                   IPL_INTER_CUBIC  Cubic convolution interpolation.
//                   IPL_SMOOTH_EDGE  Smooth edges. Can be added to
//                                    interpolation by using bitwise logical
//                                    OR (for warpFlag == IPL_WARP_R_TO_Q).
//  Notes:
*/

IPLAPI(void, iplWarpBilinear,  (IplImage* srcImage, IplImage* dstImage,
                                const double coeffs[2][4], int warpFlag,
                                int interpolate))
IPLAPI(void, iplWarpBilinearQ, (IplImage* srcImage, IplImage* dstImage,
                                const double quad[4][2], int warpFlag,
                                int interpolate))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplWarpPerspective, iplWarpPerspectiveQ
//  Purpose:        makes perspective transform of image.
//                  from rectangle to quadrangle use transform
//                      a11*j + a12*i + a13
//                  x = -------------------
//                      a31*j + a32*i + a33
//
//                      a21*j + a22*i + a23
//                  y = -------------------
//                      a31*j + a32*i + a33
//
//                        |x0-x1+x2-x3  x3-x2|   | x1-x2  x3-x2|
//                  a31 = |                  | / |             |
//                        |y0-y1+y2-y3  y3-y2|   | y1-y2  y3-y2|
//
//                        |x1-x2  x0-x1+x2-x3|   | x1-x2  x3-x2|
//                  a21 = |                  | / |             |
//                        |y1-y2  y0-y1+y2-y3|   | y1-y2  y3-y2|
//
//                  a11 = (x1-x0+a31*x1)/jmax
//                  a12 = (x3-x0+a32*x3)/imax
//                  a13 = x0
//                  a21 = (y1-y0+a31*y1)/jmax
//                  a22 = (y3-y0+a32*y3)/imax
//                  a23 = y0
//                  from quadrangle to rectangle use inverse transform
//                  |a22*a33-a32*a23  a32*a13-a12*a33  a12*a23-a22*a13|
//                  |a31*a23-a21*a33  a11*a33-a31*a13  a21*a13-a11*a23|
//                  |a21*a32-a31*a22  a31*a12-a11*a32  a11*a22-a21*a12|
//
//  Returns:        None.
//  Parameters:
//      srcImage     The source image.
//      dstImage     The resultant image.
//      quad         The vertexes of quadrangle.
//      warpFlag     If warpFlag is IPL_WARP_R_TO_Q, the transform is from 
//                   rectangle srcImage->roi to quadrangle 
//                   (x0,y0)(x1,y1)(x2,y2)(x3,y3) in dstImage.
//                   If warpFlag is IPL_WARP_Q_TO_R, the transform is from 
//                   quadrangle (x0,y0)(x1,y1)(x2,y2)(x3,y3) in srcImage to 
//                   rectangle dstImage->roi.
//      interpolate  The type of interpolation to perform for resampling
//                   the input image. The following are currently supported.
//                   IPL_INTER_NN     Nearest neighbour interpolation.
//                   IPL_INTER_LINEAR Linear interpolation.
//                   IPL_INTER_CUBIC  Cubic convolution interpolation.
//                   IPL_SMOOTH_EDGE  Smooth edges. Can be added to
//                                    interpolation by using bitwise logical
//                                    OR (for warpFlag == IPL_WARP_R_TO_Q).
//  Notes:
*/

IPLAPI(void, iplWarpPerspective, (IplImage* srcImage, IplImage* dstImage,
                                  const double coeffs[3][3], int warpFlag,
                                  int interpolate))
IPLAPI(void, iplWarpPerspectiveQ, (IplImage* srcImage, IplImage* dstImage,
                                   const double quad[4][2], int warpFlag,
                                   int interpolate))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetBilinearQuad
//  Purpose:        calculates coordinates of quadrangle from transformed 
//                  image ROI.
//  Returns:        None.
//  Parameters:
//      image       The image.
//      coeffs      The transform matrix
//                  |X|   |a11|      |a12 a13| |J|   |a14|
//                  | | = |   |*JI + |       |*| | + |   |
//                  |Y|   |a21|      |a22 a23| |I|   |a24|
//      quadr       resultant quadrangle
//  Notes:
*/

IPLAPI(void, iplGetBilinearQuad, (IplImage* image, const double coeffs[2][4],
                                  double quadr[4][2]))
IPLAPI(void, iplGetBilinearQuadROI, (IplROI* roi, const double coeffs[2][4],
                                  double quadr[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetBilinearBound
//  Purpose:        calculates bounding rectangle of the transformed image ROI.
//  Returns:        None.
//  Parameters:
//      image    The source image.
//      coeffs      The transform matrix
//                  |X|   |a11|      |a12 a13| |J|   |a14|
//                  | | = |   |*JI + |       |*| | + |   |
//                  |Y|   |a21|      |a22 a23| |I|   |a24|
//      rect        resultant bounding rectangle
//  Notes:
*/

IPLAPI(void, iplGetBilinearBound, (IplImage* image, const double coeffs[2][4],
                                   double rect[2][2]))
IPLAPI(void, iplGetBilinearBoundROI, (IplROI* roi, const double coeffs[2][4],
                                   double rect[2][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetBilinearTransform
//  Purpose:        calculates transform matrix from vertexes of quadrangle.
//  Returns:        None.
//  Parameters:
//      image       The image.
//      coeffs      The resultant transform matrix
//                  |X|   |a11|      |a12 a13| |J|   |a14|
//                  | | = |   |*JI + |       |*| | + |   |
//                  |Y|   |a21|      |a22 a23| |I|   |a24|
//      quadr       quadrangle
//  Notes:
*/

IPLAPI(void, iplGetBilinearTransform, (IplImage* image, double coeffs[2][4],
                                       const double quadr[4][2]))
IPLAPI(void, iplGetBilinearTransformROI, (IplROI* roi, double coeffs[2][4],
                                       const double quadr[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetPerspectiveQuad
//  Purpose:        calculates coordinates of quadrangle from transformed 
//                  image ROI.
//  Returns:        None.
//  Parameters:
//      image       The image.
//      coeffs      The transform matrix
//                      a11*j + a12*i + a13
//                  x = -------------------
//                      a31*j + a32*i + 1
//
//                      a21*j + a22*i + a23
//                  y = -------------------
//                      a31*j + a32*i + 1
//      quadr       resultant quadrangle
//  Notes:
*/

IPLAPI(void, iplGetPerspectiveQuad, (IplImage* image, const double coeffs[3][3],
                                     double quadr[4][2]))
IPLAPI(void, iplGetPerspectiveQuadROI, (IplROI* roi, const double coeffs[3][3],
                                     double quadr[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetPerspectiveBound
//  Purpose:        calculates bounding rectangle of the transformed image ROI.
//  Returns:        None.
//  Parameters:
//      image    The source image.
//      coeffs      The transform matrix
//                      a11*j + a12*i + a13
//                  x = -------------------
//                      a31*j + a32*i + 1
//
//                      a21*j + a22*i + a23
//                  y = -------------------
//                      a31*j + a32*i + 1
//      rect        resultant bounding rectangle
//  Notes:
*/

IPLAPI(void, iplGetPerspectiveBound,(IplImage* image, const double coeffs[3][3],
                                      double rect[2][2]))
IPLAPI(void, iplGetPerspectiveBoundROI,(IplROI* roi, const double coeffs[3][3],
                                      double rect[2][2]))



/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplGetPerspectiveTransform
//  Purpose:        calculates transform matrix from vertexes of quadrangle.
//  Returns:        None.
//  Parameters:
//      image       The image.
//      coeffs      The resultant transform matrix
//                      a11*j + a12*i + a13
//                  x = -------------------
//                      a31*j + a32*i + 1
//
//                      a21*j + a22*i + a23
//                  y = -------------------
//                      a31*j + a32*i + 1
//      quadr       quadrangle
//  Notes:
*/

IPLAPI(void, iplGetPerspectiveTransform, (IplImage* image, double coeffs[3][3],
                                          const double quadr[4][2]))
IPLAPI(void, iplGetPerspectiveTransformROI, (IplROI* roi, double coeffs[3][3],
                                          const double quadr[4][2]))


/* /////////////////////////////////////////////////////////////////////////
//  Name:           iplResize
//  Purpose:        Resize image.
//  Returns:        None.
//  Parameters:
//      srcImage         The source image.
//      dstImage         The resultant image.
//      xDst, xSrc, yDst, ySrc 
//                       They specify the fractions xDst\xSrc and yDst\ySrc.
//      interpolate      The type of interpolation to perform for resampling
//                       the input image.
//                       The following are currently supported.
//                       IPL_INTER_NN       Nearest neighbour interpolation.
//                       IPL_INTER_LINEAR   Linear interpolation.
//                       IPL_INTER_CUBIC    Cubic convolution interpolation.
//                       IPL_INTER_SUPER    Super sampling interpolation.
//  Notes:
*/

IPLAPI(void, iplResize, (IplImage* srcImage, IplImage* dstImage,
                      int xDst, int xSrc, int yDst, int ySrc, int interpolate))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplZoom
// Purpose:    Zooms or expands an image.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  xDst, xSrc,
//  yDst, ySrc  - All these should be positive integers with xDst >= xSrc and
//                yDst >= ySrc. They specify the fractions xDst/xSrc and 
//                yDst/ySrc to expand the image in the X and Y directions.
//                For example xDst=2, xSrc=1, yDst=2, ySrc=1 doubles the
//                image size in each dimension to give an image 4 times larger
//                in area.
//  interpolate - The type of interpolation to perform for resampling.
//                The following are currently supported.
//                IPL_INTER_NN            Nearest neighbour interpolation.
//                IPL_INTER_LINEAR        Linear interpolation.
//                IPL_INTER_CUBIC         Cubic convolution interpolation.
//
// Notes:      
*/

IPLAPI(void,iplZoom,(IplImage* srcImage, IplImage* dstImage, int xDst, int xSrc,
                             int yDst, int ySrc, int interpolate))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDecimate
// Purpose:    Decimates or shrinks an image.
// Returns:    
// Parameters: 
//  srcImage    - The source image.
//  dstImage    - The resultant image.
//  xDst, xSrc,
//  yDst, ySrc  - All these should be positive integers with xDst <= xSrc
//                and yDst <= ySrc. They specify the fractions xDst/xSrc
//                and yDst/ySrc to shrink the image in the X and Y directions.
//                For example xDst = 1, xSrc = 2, yDst = 1, ySrc = 2 halves
//                the image size in each dimension to give an image 1/4 times
//                smaller in area.
//  interpolate - The type of interpolation to perform for resampling.
//                The following are currently supported.
//                IPL_INTER_NN            Nearest neighbour interpolation.
//                IPL_INTER_LINEAR        Linear interpolation.
//                IPL_INTER_CUBIC         Cubic convolution interpolation.
//                IPL_INTER_SUPER         Supersampling interpolation.
//
// Notes:      
*/

IPLAPI(void, iplDecimate,(IplImage* srcImage, IplImage* dstImage, int xDst,
                 int xSrc, int yDst, int ySrc, int interpolate))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplDecimateBlur
// Purpose:    Decimation of an image with "pre-blur"
// Returns:    
// Parameters: 
//  srcImage   - the source Image.
//  dstImage   - the resultant Image.
//  xDst, xSrc,
//  yDst, ySrc - They specify the fractions xDst\xSrc and yDst\ySrc.
//  interpolate- The type of interpolation to perform for resampling.
//               The following are currently supported.
//                  IPL_INTER_NN            Nearest neighbour interpolation.
//                  IPL_INTER_LINEAR        Linear interpolation.
//                  IPL_INTER_CUBIC         Cubic convolution interpolation.
//  xMaskSize  - Number of columns in the neighbourhood(cell)to use.
//  yMaskSize  - Number of rows in the neighbourhood(cell) to use.
//               The coordinates of an anchor are in middle of a cell.
//               (In this coordinate system,
//                the top left corner would be [0, 0] and the
//                bottom right corner would be [ xMaskSize,yMaskSize].
//                For a 3 by 3 neighbourhood the coordinates
//                of the geometric center would be [1, 1]).
//
// Notes: no in-place     
*/
#define iplDecimateExt iplDecimateBlur

IPLAPI(void, iplDecimateBlur,(IplImage* srcImage,IplImage* dstImage,int xDst,
      int xSrc, int yDst, int ySrc,int interpolate,int xMaskSize,int yMaskSize)) 


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMirror
// Purpose:    Mirrors an image about a horizontal or vertical axis.
// Returns:    
// Parameters: 
//  srcImage - The source image.
//  dstImage - The resultant image.
//  flipAxis - Specifies the axis to mirror the image.
//             Use 0 for the horizontal axis,
//                 1 for a vertical axis
//                -1 for both horizontal and vertical axis.
//
// Notes:      
*/

IPLAPI(void, iplMirror,(IplImage* srcImage, IplImage* dstImage, int flipAxis))



/*==========================================================================
      Section: Image Features functions
 ==========================================================================*/

/*------------------  Definitions of the type of norms  -------------------*/
#define IPL_C   1
#define IPL_L1  2
#define IPL_L2  4
#define IPL_RELATIVE   8
#define IPL_RELATIVEC  (IPL_RELATIVE | IPL_C )
#define IPL_RELATIVEL1 (IPL_RELATIVE | IPL_L1 )
#define IPL_RELATIVEL2 (IPL_RELATIVE | IPL_L2 )

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplNorm
// Purpose:    Calculates C,L1,L2-norms for one IPL image, absolute and 
//             relative errors between two IPL-images.
// Returns:    
// Parameters: 
//   srcImageA - The source image, tiled or not, with maskROI or not.
//   srcImageB - The source image, tiled or not, with maskROI or not, may 
//               be NULL.
//   normType  - Specifies the type of norms and errors.
//
// Notes:      
//   If pointer of srcImageB is NULL,the next type of norms for 
//   srcImageA defined:
//     normType = IPL_C  for C -norm: ||A||= max ( abs(a[i,j]) );
//     normType = IPL_L1 for L1-norm: ||A||= sum ( abs(a[i,j]) )
//     normType = IPL_L2 for L2-norm: ||A||= sqrt( sum (a[i,j]**2) );
//
//   If pointer of srcImageB is not NULL, the next type of absolute errors 
//   defined:
//     normType = IPL_C  for C -absolute error: 
//          ||A-B||=max ( abs(a[i,j]-b[i,j]) );
//     normType = IPL_L1 for L1-absolute error: 
//          ||A-B||=sum ( abs(a[i,j]-b[i,j]) );
//     normType = IPL_L2 for L2-absolute error: 
//          ||A-B||=sqrt( sum (a[i,j]-b[i,j])**2 );
//
//   If pointer of srcImageB is not NULL, the next type of relative errors 
//   defined:
//     normType = IPL_RELATIVEC  for C -relative error:
//                ||A-B||/||B||= max(abs (a[i,j]-b[i,j]))/max(abs(b[i,j]));
//     normType = IPL_RELATIVEL1 for L1-relative error:
//                ||A-B||/||B||= sum(abs (a[i,j]-b[i,j]))/sum(abs(b[i,j]));
//     normType = IPL_RELATIVEL2 for L2-relative error:
//                ||A-B||/||B||= sqrt(sum((a[i,j]-b[i,j])**2)/sum((b[i,j])**2)));
*/

IPLAPI(double, iplNorm,(IplImage* srcImageA, IplImage* srcImageB, 
       int normType))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMoments
// Purpose:    Computes all image moments of order 0 to 3.
// Returns:    
// Parameters: 
//    image  - The image for which the moments will be computed.
//    mState - The structure for storing the image moments.
//
// Notes:      
*/

IPLAPI(void,iplMoments,(IplImage* img, IplMomentState stt))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplGet*Moment
// Purpose:    Get a specific moments, computed by iplMoments.
// Returns:    Specific moments  (see names of functions).
// Parameters: 
//    mState - The structure storing the image moments.
//    m, n   - The integer exponents m and n.
//             These arguments must satisfy the condition
//             0 <= (m + n) <= 3.
//
// Notes:      
*/

IPLAPI(double,iplGetSpatialMoment,(IplMomentState stt, int m, int n))
IPLAPI(double,iplGetNormalizedSpatialMoment,(IplMomentState stt, int m, int n))
IPLAPI(double,iplGetCentralMoment,(IplMomentState stt, int m, int n))
IPLAPI(double,iplGetNormalizedCentralMoment,(IplMomentState stt, int m, int n))


/* /////////////////////////////////////////////////////////////////////////
// Name:       ipl*Moment
// Purpose:    Computes a specific moments.
// Returns:    Specific moments  (see names of functions).
// Parameters: 
//     image - The image for which the moment will be computed.
//     m, n  - The integer exponents m and n.
//             These arguments must satisfy the condition
//             0 <= (m + n) <= 3.
//
// Notes:      
*/

IPLAPI(double,iplSpatialMoment,(IplImage* img, int m, int n))
IPLAPI(double,iplNormalizedSpatialMoment,(IplImage* img, int m, int n))
IPLAPI(double,iplCentralMoment,(IplImage* img, int m, int n))
IPLAPI(double,iplNormalizedCentralMoment,(IplImage* img, int m, int n))


/* /////////////////////////////////////////////////////////////////////////
// Name:       iplGreater, iplEqualFPEps, iplGreaterS, iplLessS, iplEqualS, 
//             iplGreaterSFP, iplLessSFP, iplEqualSFP, iplEqualSFPEps
// Purpose:    
// Returns:    
//     IPL_StsOk    - success
//     other status - images are not compatible, null pointer, etc.
// Parameters: 
//  Destination image should be 1u depth, one-channel image.
//        mask, ROI, and tile can be specified
//
//  Source images:
//        if two images are compared, then img1 must be compatible with img2;
//        images are compatible if their origins, depths, coi and nChannels 
//        are equal.
//        A source image can be of 1u, 8u, 8s, 16u, 16s, 32s, or 32f depth, 
//        n-channel image. mask, ROI, COI, tile can be specified. 
//        No in-place is allowed.
//        nChannels = 1..n
//
// Notes:      
//  o If (img.coi == img.alphaChannel) then 
//    alpha-channel is compared, it is the operation is executed.
//
//  o Compare operation performs "fabs( a - b ) < eps" operation in the
//    iplEqual(S)FPEps function. And it is just a direct comparison in the 
//    iplEqual(SFP) function.
//
//  o Example of the iplGreater function use:
//
//  If (img.coi == n)
//     plane: Greater(x,y) = img1.coi(x,y) > img2.coi(x,y)
//     pixel: Greater(x,y) = img1(x,y).coi > img2(x,y).coi
//
//  If (img.coi == 0) and (img.nChannels > 1) then
//     plane: Greater(x,y) = img1.red(x,y) > img2.red(x,y) && img1.green(x,y) >
//  img2.green(x,y) ... 
//     pixel: Greater(x,y) = img1(x,y).red > img2(x,y).red && img1(x,y).green >
//  img2(x,y).green ...
*/

IPLAPI(IPLStatus, iplGreater, (IplImage* img1, IplImage* img2, IplImage* res))
IPLAPI(IPLStatus, iplLess,    (IplImage* img1, IplImage* img2, IplImage* res))
IPLAPI(IPLStatus, iplEqual,   (IplImage* img1, IplImage* img2, IplImage* res))
IPLAPI(IPLStatus, iplEqualFPEps, (IplImage* img1, IplImage* img2, 
       IplImage* res, float eps))
IPLAPI(IPLStatus, iplLessS,(IplImage* img, int s, IplImage* res))
IPLAPI(IPLStatus, iplGreaterS,(IplImage* img, int s, IplImage* res))
IPLAPI(IPLStatus, iplEqualS,(IplImage* img, int s, IplImage* res))

IPLAPI(IPLStatus, iplLessSFP,(IplImage* img, float s, IplImage* res))
IPLAPI(IPLStatus, iplGreaterSFP,(IplImage* img, float s, IplImage* res))
IPLAPI(IPLStatus, iplEqualSFP,(IplImage* img, float s, IplImage* res))

IPLAPI(IPLStatus, iplEqualSFPEps,(IplImage* img, float s, IplImage* res, 
       float eps))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplMinMaxFP
// Purpose:    To find minimum and maximum value of pixels of the srcImage.
// Returns:    IPLStatus
// Parameters: 
//      srcImage - The source image.
//      min      - Pointer to minimum.
//      max      - Pointer to maximum.
//
// Notes:      
*/
IPLAPI(IPLStatus, iplMinMaxFP, (const IplImage * srcImage,
                                                      float * min, float * max))



/* /////////////////////////////////////////////////////////////////////////
// Name:       iplNormCrossCorr
// Purpose:    Produce probability image based on Normalized Cross Correlation
// Returns:    IPLStatus
// Parameters: 
//       src - source image
//       tpl - template image
//       dst - probability image based on Normalized Cross Correlation
//
// Notes:      Template image should be a non-tiled image only.
*/
IPLAPI(IPLStatus, iplNormCrossCorr,(IplImage *tpl, IplImage *src, IplImage *dst))





/*==========================================================================
      Section: Declaration of IPL Wavelet Functions 
               and Initialization Routines
 ==========================================================================*/

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtInit  
// Purpose:    Wavelet transform initialization by fixed wavelet type
//              from IplWtType enum.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     IplWtType type         - type of wavelet transform;
     int par1               - first parameter (order of wavelet transform);
     int par2               - second parameter (only for biorth. wavelets);
     IplWtKernel *wtKernel  - wavelet functions interchange structure; 
//
//  Notes:     
//
//  Wavelet transform initialization parameters:
//
//      par1, par2   -  the parameters of wavelet,
//                      dependent from the type of wavelet.
//      par1, par2   -  the parameters of wavelet,
//                      dependent from the type of wavelet.
//      IPL_WT_HAAR         par1 - dummy
//                           par2 - dummy
//      IPL_WT_DAUBLET      par1 = 1,2,3,4,5,6,7,8,9,10.
//                           par2 - dummy
//      IPL_WT_SYMMLET      par1 = 1,2,3,4,5,6,7.
//                           par2 - dummy
//      IPL_WT_COIFLET      par1 = 1,2,3,4,5.
//                           par2 - dummy
//      IPL_WT_VAIDYANATHAN par1 - dummy
//                           par2 - dummy
//
//      IPL_WT_BSPLINE      B - spline,
//      IPL_WT_BSPLINEDUAL             (par1, par2) must be:
//                          box -
//                           ( 1, 1 ), ( 1, 3 ), ( 1, 5 );
//                          lin. spline -
//                           ( 2, 2 ), ( 2, 4 ), ( 2, 6 ), ( 2, 8 );
//                          quad. spline -
//                           ( 3, 1 ), ( 3, 3 ), ( 3, 5 ), ( 3, 7 ), ( 3, 9 ).
//
//      IPL_WT_LINSPLINE    (eq. case IPL_WT_BSPLINE with par1=2, par2=2.)
//                          par1 - dummy
//                           par2 - dummy
//      IPL_WT_QUADSPLINE   (eq. case IPL_WT_BSPLINE with par1=3, par2=3.)
//                          par1 - dummy
//                           par2 - dummy
//
*/
IPLAPI(IPLStatus, iplWtInit, (IplWtType type, int par1, int par2,
       IplWtKernel *wtKernel))

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtInitUserTaps  
// Purpose:    Initialization of wavelet decomposition or reconstruction
//              by user filters. Filter parameters passed by C arrays.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     float *tap_filt[4]     - filter taps array;
     int len_filt[4]        - filter length array;
     int ofs_filt[4]        - filter offset array;
     IplWtKernel *wtKernel  - wavelet functions interchange structure;
//
//  Notes:
// 0 - low  pass decomposition  filter;
// 1 - high pass decomposition  filter;
// 2 - low  pass reconstruction filter;
// 3 - high pass reconstruction filter;    
*/
IPLAPI(IPLStatus, iplWtInitUserTaps, (float *tap_filt[4],
       int len_filt[4], int ofs_filt[4], IplWtKernel *wtKernel))

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtInitUserFilter  
// Purpose:    Initialization of wavelet decomposition or reconstruction
//              by user filters. Filter parameters passed by 
//               separate IplWtFilter structures.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     const IplWtFilter *decLow  - decompostion low-pass filter;
     const IplWtFilter *decHigh - decomposition high-pass fitler;
     const IplWtFilter *recLow  - reconstruction low-pass filter;
     const IplWtFilter *recHigh - reconstruction high-pass filter;
     IplWtKernel *wtKernel      - wavelet functions interchange structure;
//
//  Notes:     
*/
IPLAPI(IPLStatus, iplWtInitUserFilter, (const IplWtFilter *decLow,
       const IplWtFilter *decHigh, const IplWtFilter *recLow,
       const IplWtFilter *recHigh, IplWtKernel *wtKernel))

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtInitUserFilter4  
// Purpose:    Initialization of wavelet decomposition or reconstruction
//              by user filters. Filter parameters passed by 
//               IplWtFilter structures in array.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     IplWtFilter  filt[4]   - filter bank;
     IplWtKernel *wtKernel  - wavelet functions interchange structure;
//
//  Notes:     
// 0 - low  pass decomposition  filter;
// 1 - high pass decomposition  filter;
// 2 - low  pass reconstruction filter;
// 3 - high pass reconstruction filter;    
*/
IPLAPI(IPLStatus, iplWtInitUserFilter4, (IplWtFilter filt[4],
                               IplWtKernel *wtKernel))

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtDecompose
// Purpose:    One level wavelet decomposition.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     IplWtKernel *wtKernel  - wavelet functions interchange structure;
     const IplImage *src    - source image;
     IplImage *approxDst    - destination image for approximation;
     IplImage *xDetailDst   - destination image for horizontal details;
     IplImage *yDetailDst   - destination image for vertical details;
     IplImage *dDetailDst   - destination image for diagonal details;
//
//  Notes:     
*/
IPLAPI(IPLStatus, iplWtDecompose, (IplWtKernel *wtKernel, const IplImage *src,
    IplImage *approxDst, IplImage *xDetailDst, IplImage *yDetailDst,
        IplImage *dDetailDst)) 

/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtReconstruct  
// Purpose:    One level wavelet reconstruction.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     IplWtKernel *wtKernel      - wavelet functions interchange structure;
     const IplImage *approxSrc  - source approximation image;
     const IplImage *xDetailSrc - source horizontal details image;
     const IplImage *yDetailSrc - source vertical details image;
     const IplImage *dDetailSrc - source diagonal details image;
     IplImage *dst              - destination for reconstructed image;
//
//  Notes:     
*/
IPLAPI(IPLStatus, iplWtReconstruct, (IplWtKernel *wtKernel,
       const IplImage *approxSrc,  const IplImage *xDetailSrc,
       const IplImage *yDetailSrc, const IplImage *dDetailSrc, IplImage *dst))


/* //////////////////////////////////////////////////////////////////////
// Name:       iplWtFree  
// Purpose:    Final routine for wavelet transform.
//              Free internal allocated memory and reset wtKernel structure.
// Context:    
// Returns:    IPLStatus
// Parameters: 
     IplWtKernel *wtKernel  - wavelet functions interchange structure
//
//  Notes:     
*/
IPLAPI(IPLStatus, iplWtFree, (IplWtKernel *wtKernel))

/*==========================================================================
      Section: User process functions 
 ==========================================================================*/

/* /////////////////////////////////////////////////////////////////////////
// Name:     iplUserProcess
// Purpose:  Calls user IplUserFunc function for each channel of pixels 
//           which would be processed. Converts channel value to integer,
//           passes it to IplUserFunc function and saturates function result
//           to dstImage depth (except of 32s depth). To provide 32s 
//           saturation use iplUserProcessFP.
//                Supports:
//           8u, 8s, 16u, 16s, 32s depth;
//           roi, maskROI, tiled images, in-place;
//           srcImage->depth must be equal to dstImage->depth;
//           srcImage and dstImage number of channels to process must be equal.
// Returns:  None
// Parameters:
//     srcImage - source image
//     dstImage - destination image
//     cbFunc   - pointer to user function
//
// Notes:      
*/
IPLAPI(void, iplUserProcess,(IplImage* srcImage, IplImage* dstImage,
                             IplUserFunc cbFunc))

/* /////////////////////////////////////////////////////////////////////////
// Name:     iplUserProcessFP
// Purpose:  Calls user IplUserFuncFP function for each channel of pixels 
//           which would be processed. Converts channel value to float,
//           passes it to IplUserFuncFP function and saturates function result
//           to dstImage depth (except of 32f depth of course).
//                Supports:
//           8u, 8s, 16u, 16s, 32s, 32f depth;
//           roi, maskROI, tiled images, in-place;
//           srcImage->depth must be equal to dstImage->depth;
//           srcImage and dstImage number of channels to process must be equal.
// Returns:  None
// Parameters:
//     srcImage - source image
//     dstImage - destination image
//     cbFunc   - pointer to user function
//
// Notes:      
*/
IPLAPI(void, iplUserProcessFP,(IplImage* srcImage, IplImage* dstImage,
                               IplUserFuncFP cbFunc))

/* /////////////////////////////////////////////////////////////////////////
// Name:     iplUserProcessPixel
// Purpose:  Calls user IplUserFuncPixel function for each pixel which 
//           would be processed. Set src pixel and dst pixel values to arrays, 
//           passes these two arrays to IplUserFuncPixel function and test
//           IPL Error after IplUserFuncPixel return.
//                Supports:
//           8u, 8s, 16u, 16s, 32s ,32f depth;
//           rectangle roi, maskROI, tiled images;
//           srcImage->depth may not be equal to dstImage->depth;
//           srcImage->nChannels and dstImage->nChannels may be different.
//                Not supports:
//           dst saturation; it should be support by user function.
//           channel roi; it should be support by user function if necessary.
// Returns:  None
// Parameters:
//     srcImage - source image
//     dstImage - destination image
//     cbFunc   - pointer to user function
//
// Notes:      
*/
IPLAPI(void, iplUserProcessPixel,(IplImage* srcImage, IplImage* dstImage,
                                  IplUserFuncPixel cbFunc))



/*==========================================================================
      Section: Noise generators
 ==========================================================================*/

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplNoiseUniformInit,  iplNoiseUniformInitFP.
// Purpose:    Initialization of parameters for the generator of noise.
// Returns:    
// Parameters: 
//  seed     - The seed value used by the pseudo-random number generation
//             algorithm.
//  low      - The lower bounds of the uniform distribution�s range.
//  high     - The upper bounds of the uniform distribution�s range.
//
// Notes:
*/

IPLAPI(void, iplNoiseUniformInit, (IplNoiseParam* noiseParam, unsigned int seed,
                                   int low, int high))

IPLAPI(void, iplNoiseUniformInitFP, (IplNoiseParam* noiseParam,
                                     unsigned int seed, float low, float high))

/* /////////////////////////////////////////////////////////////////////////
// Name:       iplNoiseGaussianInit,  iplNoiseGaussianInitFP.
// Purpose:    Initialization of parameters for the generator of noise.
// Returns:    
// Parameters: 
//  seed     - The seed value used by the pseudo-random number generation
//             algorithm.
//  mean     - The mean of the Gaussian distribution.
//  stDev    - The standard deviation of the Gaussian distribution.
//
// Notes:
*/

IPLAPI(void, iplNoiseGaussianInit, (IplNoiseParam* noiseParam,
                                    unsigned int seed, int mean, int stDev))

IPLAPI(void, iplNoiseGaussianInitFP, (IplNoiseParam* noiseParam,
                                      unsigned int seed, float mean,
                                      float stDev))

/* /////////////////////////////////////////////////////////////////////////
// Name:         iplNoiseImage
// Purpose:      Addition of the generated noise to the image with saturation.
// Returns:      IPLStatus
// Parameters: 
//  image      - The image on which is imposed noise.
//  noiseParam - A pointer to the structure containing parameters for the
//               generator of noise.
//
// Notes:
*/

IPLAPI(IPLStatus, iplNoiseImage, (IplImage* image,
                                  const IplNoiseParam* noiseParam))




#include "iplerror.h"
#include "iplmisc.h"

#ifdef __cplusplus
}
#endif

#endif /*__IPL_H__*/
