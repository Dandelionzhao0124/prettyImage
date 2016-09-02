#include "picvalue.h"
#define CHANNEL 3

void picvalues(unsigned char* srcData, int height, int width, int stride, int& brightmean, int& brightvar) {

	int offset = stride - width*CHANNEL;
	int length = width*height;
	unsigned char *imgpt = srcData;
	int sum = 0;
	int Hist[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	/* Get the brightmean, brightvar*/	
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int pgray = (imgpt[0] * 0.299 + imgpt[1] * 0.587 + imgpt[2] * 0.114);
			sum += pgray;
			Hist[pgray]++;
			imgpt += CHANNEL;
		}
		imgpt += offset;
	}
	brightmean = sum / length - 128;//均值减去128
	float ma = 0;
	for (int i = 0; i < 256; ++i) {
		int tmp = i - 128 - brightmean;//像素值减128
		ma += (tmp > 0 ? tmp : -tmp)*Hist[i];
	}
	ma = ma / length;
	brightvar = (brightmean > 0 ? brightmean*10 : (-brightmean)*10) / (ma > 0 ? ma : -ma);//方差大于10为夜景或过曝
}