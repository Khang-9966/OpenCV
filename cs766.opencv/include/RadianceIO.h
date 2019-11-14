#ifndef _radiance_io_h_
#define _radiance_io_h_






#include "it_shared.h"



it_image_t *readRadiance( const char *file, it_image_t *img );
void        writeRadiance( const char *file, it_image_t *img );









#endif /* _radiance_io_h_ */