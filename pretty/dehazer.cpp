#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "dehazer.h"
#define min(a,b) ((a) <= (b) ? (a):(b))
#define max(a,b) ((a) >= (b) ? (a):(b))
#define CHANNEL 3

/* Initial the value used. */
void Dehazer::init(unsigned char* imageData, int height, int width, int stride, float mean, float var) {

	brightmean = mean;
	brightvar = var;

	height = height;
	width = width;
	stride = stride;

	rol = 0.2;

	/* Ratio of the height and width. */
	if (height > width) {
		radius = width / 32;
	}
	else if (height <= width) {
		radius = width / 18;
	}

	/*night. */
	if (brightmean < -50 && brightvar > 1) {
		rol = 1.8;
	}

	/*exposure over.*/
	if (brightmean > 50 && brightvar > 1) {
		rol = 0.1;
	}

	meanvalue = 0;
	beta = 3;
	MxData = new unsigned char[height*width];
	BoxblurData = new unsigned char[height*width];
	LxData = new unsigned char[height*width];
	Table = new unsigned char[height*width];
}

/*Get MxData from the srcData.
And also the mean value of the Mx.
Mx is one channel.*/
void Dehazer::MinValue(unsigned char* imageData, unsigned char* MxData, int height, int width, int stride, float& meanvalue) {

	int Min;
	int Sum = 0;
	unsigned char *imgpt = imageData;

	//row after row
	int k = 0;
	for (int i = 0; i < height; ++i) {
		imgpt = imageData + i * stride; //imgpt:the first address of every rows.
		int n = 0;
		for (int j = 0; j < width; ++j) {
			Min = min(imgpt[n], imgpt[n + 1]);
			Min = min(Min, imgpt[n + 2]);

			MxData[k] = Min;
			Sum += Min;
			n += CHANNEL;
			k += 1;
		}
	}

	meanvalue = (double)Sum / (double)(width*height * 255);

}

/* Box blur of the Mx Data. --> get the BoxblurData.*/
void Dehazer::BoxBlur(unsigned char* MxData, unsigned char* BoxblurData, int height, int width, int stride, unsigned char* imageData) {

	int k = radius * 2 + 1;//the window length
	int var = k*k;
	int sum = 0;
	int* A = new int[width];
	int gray = 0, i = 0, j = 0, m = 0, n = 0, p = 0, pn = 0;

	for (i = 0; i < height; ++i) {
		sum = 0;
		if (i == 0) {
			for (j = 0; j < width; ++j) {
				A[j] = 0;
				for (m = -radius; m <= radius; ++m) {
					n = (m < 0) ? (-m) : m;
					p = j + n * width;
					A[j] += MxData[p];
				}
				if (j < k) {
					sum += A[j];
				}
			}
		}
		else {
			for (j = 0; j < width; ++j) {
				m = (i - radius - 1) < 0 ? (abs(i - radius - 1)) : (i - radius - 1);
				n = (i + radius) >= height ? height - 1 - (i + radius) % (height - 1) : (i + radius);
				p = j + n * width;
				pn = j + m * width;
				A[j] = A[j] - MxData[pn] + MxData[p];
				if (j < k) {
					sum += A[j];
				}
			}
		}
		int tn = 0, tm = 0;
		for (j = 0; j < width; ++j) {
			if (j == 0) {
				gray = (int)(sum / var);
				gray = (gray < 255) ? gray : 255;
				p = i * width;
				BoxblurData[p] = gray;
			}
			tn = (j - 1 - radius) < 0 ? (abs(j - 1 - radius)) : (j - 1 - radius);
			tm = (j + radius) >= width ? width - 1 - (j + radius) % (width - 1) : (j + radius);
			sum = sum - A[tn] + A[tm];
			gray = (int)(sum / var);
			gray = (gray < 255) ? gray : 255;
			p = j + i * width;
			BoxblurData[p] = gray;
		}
	}
	if (A != NULL) {
		delete[] A;
		A = NULL;
	}

}

/* Get L(x) Data from M(x) Data.*/
void Dehazer::GetLxData(unsigned char* MxData, unsigned char* LxData, int height, int width, int stride, unsigned char* imageData) {

	double minvalue = 0.0;

	unsigned char *blurpt = BoxblurData;
	unsigned char *mxpt = MxData;

	int k = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			minvalue = min(rol*meanvalue, 0.9);
			minvalue = min(mxpt[k], (blurpt[k]) * minvalue);
			LxData[k] = (int)minvalue;
			k++;
		}
	}

}
/* Get the Avalue from the imgData and the BoxblurData. */
void Dehazer::GetAvalue(unsigned char* imageData, unsigned char* BoxblurData, float& Avalue, int height, int width, int stride) {

	int maxHxvalue = -1;
	int maxBlurvalue = -1;

	unsigned char *hxpt = imageData;
	unsigned char *blurpt = BoxblurData;

	int k = 0;
	for (int i = 0; i < height; ++i) {
		hxpt = imageData + i * stride;
		int n = 0;
		for (int j = 0; j < width; ++j) {
			maxHxvalue = max(maxHxvalue, hxpt[n]);
			maxHxvalue = max(maxHxvalue, hxpt[n + 1]);
			maxHxvalue = max(maxHxvalue, hxpt[n + 2]);
			maxBlurvalue = max(maxBlurvalue, blurpt[k]);

			n += CHANNEL;
			k += 1;
		}
		if (maxHxvalue == 255 && maxBlurvalue == 255) {
			break;
		}
	}
	Avalue = 0.5*(maxHxvalue*1.0 + maxBlurvalue*1.0);
}

/* Get the result FxData from the initial Data and the LxData. */
void Dehazer::GetFxData(unsigned char* imageData, unsigned char* LxData, unsigned char* Table, int height, int width, int stride) {

	unsigned char *imgpt = imageData;
	unsigned char *lxpt = LxData;

	int index, value;
	for (int i = 0; i < 256; ++i) {
		index = i << 8;
		for (int j = 0; j < 256; ++j) {
			value = (i - j) / (1 - j*(1 / Avalue));
			if (value > 255) {
				value = 255;
			}
			else if (value < 0) {
				value = 0;
			}
			Table[index++] = value;

		}
	}
	int k = 0;
	for (int i = 0; i < height; ++i) {

		imgpt = imageData + i * stride;
		int n = 0, index;
		for (int j = 0; j < width; ++j) {

			index = (imgpt[n] << 8) + lxpt[k];
			imgpt[n] = Table[index];

			index = (imgpt[n + 1] << 8) + lxpt[k];
			imgpt[n + 1] = Table[index];

			index = (imgpt[n + 2] << 8) + lxpt[k];
			imgpt[n + 2] = Table[index];

			n += CHANNEL;
			k += 1;
		}
	}

}

unsigned char* Dehazer::Dehazing(unsigned char* imageData, int height, int width, int stride, float mean, float var) {

	if (imageData != NULL)
	{
		init(imageData, height, width, stride, mean, var);
		MinValue(imageData, MxData, height, width, stride, meanvalue);
		BoxBlur(MxData, BoxblurData, height, width, stride, imageData);
		GetLxData(MxData, LxData, height, width, stride, imageData);
		GetAvalue(imageData, BoxblurData, Avalue, height, width, stride);
		GetFxData(imageData, LxData, Table, height, width, stride);
	}
	return imageData;
}