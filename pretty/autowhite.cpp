#include "autowhite.h"

void AutoWhite::init(int height, int width, int stride) {

	avga = 0, avgb = 0;
	LabData = new double[height*stride];
}
void AutoWhite::RGB2LAB(double B, double G, double R, double& L, double& a, double& b) {

	double x, y, z;
	double fx, fy, fz;
	double BLACK = 20.0, YELLOW = 70.0;

	x = 0.412453*R + 0.357580*G + 0.180423*B;
	y = 0.212671*R + 0.715160*G + 0.072169*B;
	z = 0.019334*R + 0.119193*G + 0.950227*B;

	x = x / (255.0*0.950456);
	y = y / 255.0;
	z = z / (255.0*1.088754);

	if (y > 0.008856) {
		fy = pow(y, 1.0 / 3.0);
		L = 116.0*fy - 16.0;
	}
	else {
		fy = 7.787*y + 16.0 / 116.0;
		L = 903.3*y;
	}
	if (x > 0.008856) {
		fx = pow(x, 1.0 / 3.0);
	}
	else {
		fx = 7.787*x + 16.0 / 116.0;
	}
	if (z > 0.008856) {
		fz = pow(z, 1.0 / 3.0);
	}
	else {
		fz = 7.787*z + 16.0 / 116.0;
	}

	a = 500.0*(fx - fy);
	b = 200.0*(fy - fz);

	if (L < BLACK)
	{
		a *= exp((L - BLACK) / (BLACK / 4));
		b *= exp((L - BLACK) / (BLACK / 4));
		L = 20;
	}
	if (b > YELLOW)b = YELLOW;

	// ¹éÒ»»¯ÖµLab
	L = L / 255.0;    // L
	a = (a + 128.0) / 255.0; // a
	b = (b + 128.0) / 255.0; // b
}
void AutoWhite::LAB2RGB(double L, double a, double b, BYTE& B, BYTE& G, BYTE& R) {

	double fx, fy, fz;
	double x, y, z;
	int btmp, gtmp, rtmp;

	L = L*255.0;
	a = a*255.0 - 128.0;
	b = b*255.0 - 128.0;

	fy = (L + 16.0) / 116.0;
	fy = fy*fy*fy;

	if (fy > 0.008856) {
		y = fy;
	}
	else {
		fy = L / 903.3;
	}
	if (fy > 0.008856) {
		fy = pow(fy, 1.0 / 3.0);
	}
	else {
		fy = 7.787*fy + 16.0 / 116.0;
	}
	fx = a / 500.0 + fy;
	if (fx > 0.206893) {
		x = pow(fx, 3.0);
	}
	else {
		x = (fx - 16.0 / 116.0) / 7.787;
	}

	fz = fy - b / 200.0;
	if (fz > 0.206893) {
		z = pow(fz, 3);
	}
	else {
		z = (fz - 16.0 / 116.0) / 7.787;
	}

	x = x*0.950456*255.0;
	y = y*255.0;
	z = z*1.088754*255.0;

	// [ R ]   [  3.240479 -1.537150 -0.498535 ]   [ X ]
	// [ G ] = [ -0.969256  1.875992  0.041556 ] * [ Y ]
	// [ B ]   [  0.055648 -0.204043  1.057311 ]   [ Z ]
    rtmp = (int)(3.240479*x - 1.537150*y - 0.498535*z + 0.5);
	gtmp = (int)(-0.969256*x + 1.875992*y + 0.041556*z + 0.5);
	btmp = (int)(0.055648*x - 0.204043*y + 1.057311*z + 0.5);
    
	rtmp = rtmp > 255 ? 255 : rtmp;
	rtmp = rtmp < 0 ? 0 : rtmp;
	gtmp = gtmp > 255 ? 255 : gtmp;
	gtmp = gtmp < 0 ? 0 : gtmp;
	btmp = btmp > 255 ? 255 : btmp;
	btmp = btmp < 0 ? 0 : btmp;

	R = rtmp;
	G = gtmp;
	B = btmp;
}

void AutoWhite::GetAvg(BYTE* imgData, double* LabData, int height, int width, int stride) {

	avga = 0, avgb = 0;
	double suma = 0, sumb = 0;

	BYTE* imgpt = imgData;
	double* labpt = LabData;

	for (int i = 0; i < height; ++i) {
		imgpt = imgData + i*stride;
		labpt = LabData + i*stride;
		int n = 0;
		for (int j = 0; j < width; ++j) {
			RGB2LAB((double)imgpt[n], (double)imgpt[n + 1], (double)imgpt[n + 2], labpt[n], labpt[n + 1], labpt[n+2]);
			suma += labpt[n + 1];
			sumb += labpt[n + 2];
			n += 3;
		}
	}
	avga = suma / (height*width);
	avgb = sumb / (height*width);
}

void AutoWhite::Shift(double* LabData, BYTE* imgData, int height, int width, int stride, double shifta, double shiftb) {

	double* labpt = LabData;
	BYTE* imgpt = imgData;
	double delta_a, delta_b;
	for (int i = 0; i < height; ++i) {
		labpt = LabData + i*stride;
		imgpt = imgData + i*stride;
		int n = 0;
		for (int j = 0; j < width; ++j) {
			delta_a = shifta*0.002*1.1;
			delta_b = shiftb*0.002*1.1;
			labpt[n + 1] += delta_a;
			labpt[n + 2] += delta_b;

			LAB2RGB(labpt[n], labpt[n + 1], labpt[n + 2], imgpt[n], imgpt[n + 1], imgpt[n + 2]);
			n += 3;
		}
	}	
}

BYTE* AutoWhite::GrayWorldAssumption(BYTE* imgData, int height, int width, int stride) {

	init(height, width, stride);
	GetAvg(imgData, LabData, height, width, stride);
	Shift(LabData, imgData, height, width, stride, -avga, -avgb);


	return imgData;
}

