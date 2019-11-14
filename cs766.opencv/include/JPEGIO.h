#ifndef _jpeg_io_h_
#define _jpeg_io_h_





#include "it_shared.h"



it_image_t *readJPEG( const char *file, it_image_t *img );
void        writeJPEG( const char *file, it_image_t *img );










#endif /* _jpeg_io_h_ */