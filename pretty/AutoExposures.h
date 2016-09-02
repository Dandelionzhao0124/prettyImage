#ifndef AUTO_EXPOSURES_H
#define AUTO_EXPOSURES_H
#include <math.h>
#include <time.h>

#include "picture.h"

unsigned char* AutoExposures(unsigned char* srcData, int height, int width, int stride, int mean, int var);

#endif