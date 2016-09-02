#include "sharpen.h"
#include "canny.h"
#define min2(a,b) ((a) < (b) ? (a):(b))
#define max2(a,b) ((a) > (b) ? (a):(b))
#define CLIP3(x, a, b) min(max(a,x), b)
#define CHANNEL 3

#include <opencv2/opencv.hpp>                                                                                                        
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

using namespace cv;

void showgrayu(unsigned char*grayData, int width, int height) {
	Mat result(height, width, CV_8UC1);
	unsigned char* tmp = grayData;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			result.at<uchar>(i, j) = tmp[0];
			tmp += 1;
		}
	}
	imwrite("F:/img2/resultgray.jpg", result);
}
unsigned char* f_TSobelFilter(unsigned char *srcData, int width, int height)
{
	unsigned char *dstData = new unsigned char[height*width];
	if (NULL == srcData)
		return srcData;
	int x, y, i, k, pos;
	int hValue, vValue;
	unsigned char *pSrcL0;
	unsigned char *pSrcL1;
	unsigned char *pSrcL2;
	unsigned char *pDstL;
	unsigned char SqrtValue[65026];
	memset(dstData, 0, height * width);
	pSrcL0 = srcData;
	pSrcL1 = srcData + width;
	pSrcL2 = srcData + width * 2;
	pDstL = dstData + width;
	for (i = 0; i < 65026; i++)
	{
		SqrtValue[i] = (unsigned char)(sqrt((float)i));
	}
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			pos = x*3 + width;
			hValue = (-pSrcL0[pos - 4] + pSrcL0[pos + 4] - 2 * pSrcL1[pos - 4] + 2 * pSrcL1[pos + 4] - pSrcL2[pos - 4] + pSrcL2[pos + 4]);
			vValue = (pSrcL0[pos - 4] + 2 * pSrcL0[pos] + pSrcL0[pos + 4] - pSrcL2[pos - 4] - 2 * pSrcL2[pos] - pSrcL2[pos + 4]);
			k = hValue * hValue + vValue * vValue;
			k = min2(k, 65025);
			pDstL[pos] = SqrtValue[k];
			hValue = (-pSrcL0[pos - 3] + pSrcL0[pos + 5] - 2 * pSrcL1[pos - 3] + 2 * pSrcL1[pos + 5] - pSrcL2[pos - 3] + pSrcL2[pos + 5]);
			vValue = (pSrcL0[pos - 3] + 2 * pSrcL0[pos + 1] + pSrcL0[pos + 5] - pSrcL2[pos - 3] - 2 * pSrcL2[pos + 1] - pSrcL2[pos + 5]);
			k = hValue * hValue + vValue * vValue;
			k = min2(k, 65025);
			pDstL[pos + 1] = SqrtValue[k];
			hValue = (-pSrcL0[pos - 2] + pSrcL0[pos + 6] - 2 * pSrcL1[pos - 2] + 2 * pSrcL1[pos + 6] - pSrcL2[pos - 2] + pSrcL2[pos + 6]);
			vValue = (pSrcL0[pos - 2] + 2 * pSrcL0[pos + 2] + pSrcL0[pos + 6] - pSrcL2[pos - 2] - 2 * pSrcL2[pos + 2] - pSrcL2[pos + 6]);
			k = hValue * hValue + vValue * vValue;
			k = min2(k, 65025);
			pDstL[pos + 2] = SqrtValue[k];
			pDstL[pos + 3] = pSrcL1[pos + 3];
		}
		pSrcL0 += width;
		pSrcL1 += width;
		pSrcL2 += width;
		pDstL += width;
	}
	return dstData;
}

unsigned char* sharpen(unsigned char* srcData, int height, int width, int stride) {

	unsigned char* grayData = new unsigned char[height*width];
	unsigned char* result = new unsigned char[height*width];
	unsigned char* gaussData = new unsigned char[height*width];
	float* depthData = new float[height*width];
	unsigned char* strong = new unsigned char[height*width];
	unsigned char* weak = new unsigned char[height*width];

	unsigned char* presult = result;
	unsigned char* pgray = grayData;
	unsigned char* psrc = srcData;
	float* pdepth = depthData;

	int offset = stride - width * CHANNEL;
	int k = 6;//2-10

	//gray data
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			pgray[0] = (psrc[0] * 117 + psrc[1] * 601 + psrc[2] * 306 + 512) >> 10;
			psrc += CHANNEL;
			pgray += 1;
		}
		psrc += offset;
	}	

	strong = canny(grayData, width, height);
	unsigned char* pstrong = strong;
	weak = f_TSobelFilter(grayData, width, height);
	unsigned char* pweak = weak;

	//showgrayu(strong, width, height);
	//showgrayu(weak, width, height);

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			presult[0] = max((pweak[0] - pstrong[0]), 0);
			pdepth[0] = max((presult[0] / 255.0) / k, 0.0);
			presult++;
			pweak++;
			pstrong++;
			pdepth++;
		}
	}

	gaussData = gaussianBlur(grayData, width, height);
	unsigned char* pgauss = gaussData;
	

	psrc = srcData;
	pdepth = depthData;

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			psrc[0] = min2(max2((pdepth[0] + 1)*psrc[0] - pdepth[0] * pgauss[0], 0), 255);
			psrc[1] = min2(max2((pdepth[0] + 1)*psrc[1] - pdepth[0] * pgauss[0], 0), 255);
			psrc[2] = min2(max2((pdepth[0] + 1)*psrc[2] - pdepth[0] * pgauss[0], 0), 255);

			psrc += CHANNEL;
			pdepth++;
			pgauss++;
		}
		psrc += offset;
	}
	return srcData;
}
