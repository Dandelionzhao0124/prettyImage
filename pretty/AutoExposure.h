#ifndef AUTO_EXPOSURE_H
#define AUTO_EXPOSURE_H
#include <math.h>
#include "segment/pnmfile.h"
#include "segment/segment-image.h"
#include "picture.h"
#include "canny.h"

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CHANNEL 3

unsigned char* AutoExposure(unsigned char* srcData, int height, int width, int stride);



#endif