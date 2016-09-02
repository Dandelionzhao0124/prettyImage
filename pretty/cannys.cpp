#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "cannys.h"
//为sharpen服务的canny
float  **thetas;
float* edge_magnitude;
int* edge_direction;
float** magArray;

void init(unsigned char* grayData, int width, int height){

	thetas = new float *[height];
	for (int i = 0; i < height; i++) {
		thetas[i] = new float[width];
	}

	edge_magnitude = new float[height*width];
	edge_direction = new int[height*width];
}

//***************************
// convolve is a general helper funciton that applies a convolution
// to the image and then returns the weighted sum so that
// it can replace whatever pixel we were just analyzing
//**************************
float convolve(unsigned char* grayData, int width, int height, int con[][MAX_SIZE], int dim, float divisor, int i, int j) {
	int midx = dim / 2;
	int midy = dim / 2;

	float weightedSum = 0;
	for (int y = j - midy; y < j + dim - midy; y++) {
	    for (int x = i - midx; x < i + dim - midx; x++) {		
			weightedSum += divisor*(double)(con[x - i + midx][y - j + midy] * grayData[y*width+x]);
		}
	}
	return weightedSum;
}


//*****************************
// gaussian blur
// applies a gaussian blur via a convolution of a gaussian
//*****************************
unsigned char* gaussianBlur(unsigned char* grayData, int width, int height) {
	
	unsigned char* result = new unsigned char[height*width];
	int templates[9] = 
	{   1, 2, 1,
		2, 4, 2,
		1, 2, 1 };

	memcpy(result, grayData, width*height*sizeof(unsigned char));
	for (int j = 1; j < height - 1; j++)
	{
		for (int i = 1; i < width - 1; i++)
		{
			int sum = 0;
			int index = 0;
			for (int m = j - 1; m < j + 2; m++)
			{
				for (int n = i - 1; n < i + 2; n++)
				{
					sum += grayData[m*width + n] * templates[index++];
				}
			}
			sum /= 16;
			if (sum > 255)
				sum = 255;
			result[j*width + i] = sum;
		}
	}
	return result;
}


//****************************
// Applies a sobel filter to find the gradient direction
// and magnitude. those values are then stored in thetas and magArray
// so that info can be used later for further analysis
//****************************
unsigned char* sobel(unsigned char* grayData, int width, int height) {
	
	unsigned char* result = new unsigned char[height*width];
	unsigned char* presult = result;

	float Gx[9] =
	{
		1.0, 0.0, -1.0,
		2.0, 0.0, -2.0,
		1.0, 0.0, -1.0,
	};
	float Gy[9] =
	{
		-1.0, -2.0, -1.0,
		0.0, 0.0, 0.0,
		1.0, 2.0, 1.0,
	};
	float value_gx, value_gy;

	float max = 0.0;
	float angle = 0.0;

	// Convolution.
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			value_gx = 0.0;
			value_gy = 0.0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					value_gx += Gx[l * 3 + k] * grayData[(j + 1 + 1 - k)*width + (i + 1) + (1 - l)];
					value_gy += Gy[l * 3 + k] * grayData[(j + 1 + 1 - k)*width + (i + 1) + (1 - l)];
				}
			}

			edge_magnitude[j * width + i] = sqrt(value_gx * value_gx + value_gy * value_gy) / 4.0;

			// Maximum magnitude.
			max = edge_magnitude[j * width + i] > max ? edge_magnitude[j * width + i] : max;

			// Angle calculation.
			if ((value_gx != 0.0) || (value_gy != 0.0)) {
				angle = atan2(value_gy, value_gx) * 180.0 / PI;
			}
			else {
				angle = 0.0;
			}
			if (((angle > -22.5) && (angle <= 22.5)) ||
				((angle > 157.5) && (angle <= -157.5))) {
				edge_direction[j * width + i] = 0;
			}
			else if (((angle > 22.5) && (angle <= 67.5)) ||
				((angle > -157.5) && (angle <= -112.5))) {
				edge_direction[j * width + i] = 45;
			}
			else if (((angle > 67.5) && (angle <= 112.5)) ||
				((angle > -112.5) && (angle <= -67.5))) {
				edge_direction[j * width + i] = 90;
			}
			else if (((angle > 112.5) && (angle <= 157.5)) ||
				((angle > -67.5) && (angle <= -22.5))) {
				edge_direction[j * width + i] = 135;
			}
		}
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			edge_magnitude[j * width + i] =
				255.0f * edge_magnitude[j * width + i] / max;
			presult[j*width+i] =  edge_magnitude[j * width + i];
		}
	}
	return presult;
}

//***************************
// helper function that returns true if a>b and c
//***************************
bool isFirstMax(int a, int b, int c){
	if (a>b && a>c){
		return true;
	}
	return 0;
}

//****************************
// buckets the thetas into 0, 45, 90, 135
//****************************

int getOrientation(float angle) {
	if (isBetween(angle, -22.5, 22.5, -180, -157.5) || isBetween(angle, 157.5, 180, -22.5, 0))
		return 0;
	if (isBetween(angle, 22.5, 67.5, -157.5, -112.5))
		return 45;
	if (isBetween(angle, 67.5, 112.5, -112.5, -67.5))
		return 90;
	if (isBetween(angle, 112.5, 157.5, -67.5, -22.5))
		return 135;

	return -1;

}

//*****************************
//helper function that says whether arg is between a-b or c-d
//*****************************
bool isBetween(float arg, float a, float b, float c, float d) {
	if ((arg >= a && arg <= b) || (arg >= c && arg <= d)) {
		return true;
	}
	else {
		return false;
	}
}

//*****************************
//non-maximum suppression 
//depending on the orientation, pixels are either thrown away or accepted
//by checking it's neighbors
//*****************************
unsigned char* noMax(unsigned char* grayData, int width, int height) {
	
	float pixel_1 = 0;
	float pixel_2 = 0;
	float pixel;

	for (int j = 1; j < height - 1; j++) {
		for (int i = 1; i < width - 1; i++) {
			if (edge_direction[j * width + i] == 0) {
				pixel_1 = edge_magnitude[(j + 1) * width + i];
				pixel_2 = edge_magnitude[(j - 1) * width + i];
			}
			else if (edge_direction[j * width + i] == 45) {
				pixel_1 = edge_magnitude[(j + 1) * width + i - 1];
				pixel_2 = edge_magnitude[(j - 1) * width + i + 1];
			}
			else if (edge_direction[j * width + i] == 90) {
				pixel_1 = edge_magnitude[j * width + i - 1];
				pixel_2 = edge_magnitude[j * width + i + 1];
			}
			else if (edge_direction[j * width + i] == 135) {
				pixel_1 = edge_magnitude[(j + 1) * width + i + 1];
				pixel_2 = edge_magnitude[(j - 1) * width + i - 1];
			}
			pixel = edge_magnitude[j * width + i];
			if ((pixel >= pixel_1) && (pixel >= pixel_2)) {
				grayData[j*width + i] = pixel;
			}
			else {
				grayData[j*width + i] = 0;
			}
		}
	}

	bool change = true;
	while (change) {
		change = false;
		for (int j = 1; j < height - 1; j++) {
			for (int i = 1; i < width - 1; i++) {
				if (grayData[j * width + i] == 255) {
					if (grayData[(j + 1) * width + i] == 128) {
						change = true;
						grayData[(j + 1) * width + i] = 255;
					}
					if (grayData[(j - 1) * width + i] == 128) {
						change = true;
						grayData[(j - 1) * width + i] = 255;
					}
					if (grayData[j * width + (i+1)] == 128) {
						change = true;
						grayData[j * width + (i + 1)] =  255;
					}
					if (grayData[j * width + (i - 1)] == 128) {
						change = true;
						grayData[j * width + (i - 1)] =  255;
					}
					if (grayData[(j + 1) * width + (i + 1)] == 128) {
						change = true;
						grayData[(j + 1) * width + (i + 1)] =  255;
					}
					if (grayData[(j - 1) * width + (i - 1)] == 128) {
						change = true;
						grayData[(j - 1) * width + (i - 1)] =  255;
					}
					if (grayData[(j - 1) * width + (i + 1)] == 128) {
						change = true;
						grayData[(j - 1) * width + (i + 1)] =  255;
					}
					if (grayData[(j + 1) * width + (i - 1)] == 128) {
						change = true;
						grayData[(j + 1) * width + (i - 1)] =  255;
					}
				}
			}
		}
		if (change) {
			for (int j = height - 2; j > 0; j--) {
				for (int i = width - 2; i > 0; i--) {
					if (grayData[j * width + i] == 255) {
						if (grayData[(j + 1) * width + i] == 128) {
							change = true;
							grayData[(j + 1) * width + i] = 255;
						}
						if (grayData[(j - 1) * width + i] == 128) {
							change = true;
							grayData[(j - 1) * width + i] = 255;
						}
						if (grayData[j * width + (i + 1)] == 128) {
							change = true;
							grayData[j * width + (i + 1)] = 255;
						}
						if (grayData[j * width + (i - 1)] == 128) {
							change = true;
							grayData[j * width + (i - 1)] = 255;
						}
						if (grayData[(j + 1) * width + (i + 1)] == 128) {
							change = true;
							grayData[(j + 1) * width + (i + 1)] = 255;
						}
						if (grayData[(j - 1) * width + (i - 1)] == 128) {
							change = true;
							grayData[(j - 1) * width + (i - 1)] = 255;
						}
						if (grayData[(j - 1) * width + (i + 1)] == 128) {
							change = true;
							grayData[(j - 1) * width + (i + 1)] = 255;
						}
						if (grayData[(j + 1) * width + (i - 1)] == 128) {
							change = true;
							grayData[(j + 1) * width + (i - 1)] = 255;
						}
					}
				}
			}
		}
	}

	// Suppression
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (grayData[j * width + i] == 128) {
				grayData[j * width + i] = 0;
			}
		}
	}
	return grayData;
}


//*******************************
//hysteresis noise filter makes lines continuous and filters out the noise
// see the pdf that we used to understand this step in english (Step 5)
//*******************************
void hysteresisRecursion(long x, long y, int lowThresh, int width, int height, unsigned char* grayData) {

	int value = 0;

	for (long x1 = x - 1; x1 <= x + 1; x1++) {
		for (long y1 = y - 1; y1 <= y + 1; y1++) {
			if ((x1 < height) & (y1 < width) & (x1 >= 0) & (y1 >= 0)
				& (x1 != x) & (y1 != y)) {

				value = grayData[x1*width+ y1];
				if (value != 255) {
					if (value >= lowThresh) {
						grayData[x1*width+y1] =  255;
						hysteresisRecursion(x1, y1, lowThresh, width, height, grayData);
					}
					else {
						grayData[x1*width+y1] = 0;
					}
				}
			}
		}
	}
}
unsigned char* hysteresis(unsigned char* grayData, int width, int height, int lowThresh, int highThresh) {
	
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (grayData[x*width+y] >= highThresh) {
				grayData[x*width + y] = 255;
				hysteresisRecursion(x, y, lowThresh, width, height, grayData);
			}
		}
	}

	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (grayData[x*width+y] != 255) {
				grayData[x*width + y] =  0;
			}
		}
	}
	return grayData;
}

//***********************************
// the Canny Edge Detection algorithm
//***********************************
unsigned char* canny(unsigned char* grayData, int width, int height) {

	int lowThresh = 3;
	int highThresh = 12;

	unsigned char* cannyresult = new unsigned char[height*width];

	init(grayData, width, height);
	cannyresult = gaussianBlur(grayData, width, height);
	cannyresult = sobel(cannyresult, width, height);
	cannyresult = noMax(cannyresult, width, height);
	cannyresult = hysteresis(cannyresult, width, height, lowThresh, highThresh);

	return cannyresult;
}




