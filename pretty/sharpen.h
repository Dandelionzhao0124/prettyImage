#ifndef SHARPEN_H
#define SHARPEN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

unsigned char* sharpen(unsigned char* srcData, int height, int width, int stride);

#endif