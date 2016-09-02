#ifndef CANNY_H
#define CANNY_H
#include <math.h>
#define MAX_SIZE 5
#define PI 3.14159
#define T_LOW 65
#define T_HIGH 80

void init(unsigned char* grayData, int width, int height);
float convolve(unsigned char* grayData, int width, int height, int con[][MAX_SIZE], int dim, float divisor, int i, int j);
unsigned char* gaussianBlur(unsigned char* grayData, int width, int height);
unsigned char* sobel(unsigned char* grayData, int width, int height);
bool isFirstMax(int a, int b, int c);
int getOrientation(float angle);
bool isBetween(float arg, float a, float b, float c, float d);
unsigned char* noMax(unsigned char* grayData, int width, int height);
void hysteresis(unsigned char* grayData, int width, int height);
unsigned char* canny(unsigned char* grayData, int width, int height);


#endif