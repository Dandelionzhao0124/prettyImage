#ifndef AUTO_WHITE_H
#define AUTO_WHITE_H
#include "picture.h"
#include <iostream>
#include <cstdlib>

class AutoWhite{

public:
	BYTE* GrayWorldAssumption(BYTE* imgData, int height, int width, int stride);
	~AutoWhite() {
		if (LabData != NULL) {
			delete[] LabData;
			LabData = NULL;
		}		
	}
private:
	double avga, avgb;
	double* LabData;
	void init(int height, int width, int stride);
	void GetAvg(BYTE* imgData, double* LabData, int height, int width, int stride);
	void Shift(double* LabData, BYTE* imgData, int height, int width, int stride, double shifta, double shiftb);
	void RGB2LAB(double B, double G, double R, double& L, double& a, double& b);
	void LAB2RGB(double L, double a, double b, BYTE& B, BYTE& G, BYTE& R);

};

#endif