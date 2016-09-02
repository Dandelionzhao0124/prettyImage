#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <time.h>
#include "picture.h"
#include "dehazer.h"
#include "saturater.h"
#include "autowhite.h"
#include "AutoAddBright.h"
#include "AutoColorLevel.h"
#include "AutoExposure.h"
#include "AutoExposures.h"
#include "sharpen.h"
#include "picvalue.h"

#define min(a,b) ((a) < (b) ? (a):(b))
#include <opencv2/opencv.hpp>                                                                                                        
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: program_name <file_in> <file_out_3channels> <file_out_gray>\n");
		return 1;
	}
	clock_t t1, t2;

	/* Read image files. */
	char* inputfilename = argv[1];
	char* outputfilename = argv[2];

    Picture* pic = new Picture;
	pic->Read(inputfilename);

	
	/* Image mean var values.*/
	int mean = 0;
	int var = 0;
	picvalues(pic->imgData, pic->height, pic->width, pic->stride, mean, var);

	/* Image Dehazing. */ //去雾
	if (mean < -30 && var > 10) {
		Dehazer dehz;
		pic->imgData = dehz.Dehazing(pic->imgData, pic->height, pic->width, pic->stride, mean, var);
	}

	/* Image auto exposure correction. */ //自动亮度
	if (mean < -20 && var < 10) {
		pic->imgData = AutoExposure(pic->imgData, pic->height, pic->width, pic->stride);
	}

	///*自动亮度简化*/
	///* Image auto exposure correction. */ //not the night scene.
	//if (mean < 0 && mean > -35 && var < 10) {
	//pic->imgData = AutoExposures(pic->imgData, pic->height, pic->width, pic->stride, mean, var);
	//}

	/*auto color level*///自动色阶
	pic->imgData = AutoColorLevel(pic->imgData, pic->height, pic->width, pic->stride);


	/* Image Auto Saturation*///饱和度
	Saturater satur;
	int nParameter = 30;
	pic->imgData = satur.saturating(pic->imgData, pic->height, pic->width, pic->stride, nParameter);


	///*AutoAddBright*///自动补光
	if (mean < 0 && var < 10) {
		int ratio = -pic->brightmean;
		pic->imgData = AutoAddBright(pic->imgData, pic->height, pic->width, pic->stride, ratio);
	}

	pic->imgData = sharpen(pic->imgData, pic->height, pic->width, pic->stride);

	cout << t2 - t1 << endl;

	/* Write image result. */
	pic->Write(outputfilename);

	return 0;
}
