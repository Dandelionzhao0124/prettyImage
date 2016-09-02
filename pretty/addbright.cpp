#include "addbright.h"
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CLIP3(x, a, b) MIN2(MAX2(a,x), b)

BYTE* AddBright::Bright(unsigned char* imgData, int width, int height, int stride, int ratio) {
	int res = 0;
	int i, j, avgColorR = 0, avgColorG = 0, avgColorB = 0, Gray, colorMap1, colorScreen, colorScreen2, alpha, versionGray = 150;

	int len = width * height;
	BYTE* imgpt = imgData;
	
	BYTE* Map = new BYTE[256];
	for (int i = 0; i < 256; ++i) {
		Map[i] = i;
	}

	for (i = 0; i < height; i++)
	{
		imgpt = imgData + i*stride;
		int n = 0;
		for (j = 0; j < width; j++)
		{
			avgColorB += imgpt[n];
			avgColorG += imgpt[n + 1];
			avgColorR += imgpt[n + 2];
			n += 3;
		}
	}

	avgColorB = avgColorB / len;
	avgColorG = avgColorG / len;
	avgColorR = avgColorR / len;

	Gray = (avgColorR * 306 + avgColorG * 601 + avgColorB * 117 + 512) >> 10;
	colorMap1 = Map[Gray];
	colorScreen = (255 - ((255 - Gray)*(255 - colorMap1)) / 255);
	alpha = MIN2(128, MAX2(0, (versionGray - Gray) * 128 / (colorMap1 - Gray)));

	ratio = ratio * 128 / 100;

	for (i = 0; i < height; i++)
	{
		imgpt = imgData + i * stride;
		int n = 0;
		for (j = 0; j < width; j++)
		{
			colorScreen2 = ((255 - ((255 - imgpt[n])*(255 - Map[imgpt[n]])) / 255));
			colorScreen2 = (BYTE)((alpha * colorScreen2 + (128 - alpha) * imgpt[n]) >> 7);
			imgpt[n] = (colorScreen2 * ratio + (128 - ratio) * imgpt[n]) >> 7;

			colorScreen2 = ((255 - ((255 - imgpt[n+1]) * (255 - Map[imgpt[n+1]])) / 255));
			colorScreen2 = (BYTE)((alpha * colorScreen2 + (128 - alpha) * imgpt[n+1]) >> 7);
			imgpt[n+1] = (colorScreen2 * ratio + (128 - ratio) * imgpt[n+1]) >> 7;

			colorScreen2 = ((255 - ((255 - imgpt[n+2]) * (255 - Map[imgpt[n+2]])) / 255));
			colorScreen2 = (BYTE)((alpha * colorScreen2 + (128 - alpha) * imgpt[n+2]) >> 7);
			imgpt[n+2] = (colorScreen2 * ratio + (128 - ratio) * imgpt[n+2]) >> 7;

			n += 4;
		}
	}
	return imgData;
}
