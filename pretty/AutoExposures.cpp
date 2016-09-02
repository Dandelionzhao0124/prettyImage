#include "AutoExposure.h"
#define CHANNEL 3

unsigned char* AutoExposures(unsigned char* srcData, int height, int width, int stride, int mean, int var) {

	int offset = stride - width * CHANNEL;
	unsigned char* pSrc = srcData;

	int i, j;

	float scale = 1 + (-mean)*0.18 / 35.0;
	float r = 0, g = 0, b = 0;

	//gray and normalize
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {

			b = 0.815*pSrc[0] - 0.0001*pSrc[1] + 0.185*pSrc[2];
			g = -0.185*pSrc[0] + 1.0*pSrc[1] + 0.185*pSrc[2];
			r = -0.185*pSrc[0] - 0.0003*pSrc[1] + 1.185*pSrc[2];

			b = b * scale;
			g = g * scale;
			r = r * scale;

			b = (b > 255) ? 255 : b;
			b = (b < 0) ? 0 : b;
			g = (g > 255) ? 255 : g;
			g = (g < 0) ? 0 : g;
			r = (r > 255) ? 255 : r;
			r = (r < 0) ? 0 : r;

			pSrc[0] = (b > 255) ? 255 : b;
			pSrc[0] = (b < 0) ? 0 : b;

			pSrc[1] = (g > 255) ? 255 : g;
			pSrc[1] = (g < 0) ? 0 : g;

			pSrc[2] = (r > 255) ? 255 : r;
			pSrc[2] = (r < 0) ? 0 : r;
			
			pSrc += CHANNEL;
		}
		pSrc += offset;
	}

	return srcData;
}