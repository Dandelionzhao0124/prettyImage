#ifndef SATURATER_H
#define SATURATER_H

#include "picture.h"
#define min(a,b) ((a) <= (b) ? (a):(b))
#define max(a,b) ((a) >= (b) ? (a):(b))

class Saturater {
private:
	void GetValue(BYTE& b, BYTE& g, BYTE& r, double dPercent);
public:
	BYTE* saturating(BYTE *imgData, int height, int width, int stride, int nParameter);
};

#endif