#ifndef _targa_io_h_
#define _targa_io_h_


#include "it_shared.h"



it_image_t *readTarga( const char *file, it_image_t *img );
void        writeTarga( const char *file, it_image_t *img );







#endif /* _targa_io_h_ */