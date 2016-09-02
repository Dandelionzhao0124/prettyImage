#include "saturater.h"

void Saturater::GetValue(BYTE& b, BYTE& g, BYTE& r, double dPercent) {

	BYTE rgbMax;
	BYTE rgbMin;
	double  dDelta;
	double  dValue;
	double  dL;
	double  dS;
	double  dAlpha;
	
	rgbMax = max(max(b, g), r);
	rgbMin = min(min(b, g), r);

	dDelta = static_cast<double>(rgbMax - rgbMin) / 255.0;
	dValue = static_cast<double>(rgbMax + rgbMin) / 255.0;

	if (dDelta == 0) {
		return ;
	}
	dL = dValue / 2;
	if (dL < 0.5) {
		dS = dDelta / dValue;
	}
	else {
		dS = dDelta / (2.0 - dValue);
	}
	if (dPercent >= 0) {
		if (dPercent + dS >= 1) {
			dAlpha = dS;
		}
		else {
			dAlpha = 1 - dPercent;
		}
		dAlpha = 1.0 / dAlpha - 1.0;
		b = b + (b - dL * 255)*dAlpha;
		g = g + (g - dL * 255)*dAlpha;
		r = r + (r - dL * 255)*dAlpha;
	}
	else {
		dAlpha = dPercent;
		b = dL * 255 + (b - dL * 255)*(1.0 + dAlpha);
		g = dL * 255 + (g - dL * 255)*(1.0 + dAlpha);
		r = dL * 255 + (r - dL * 255)*(1.0 + dAlpha);
	}
}

BYTE* Saturater::saturating(BYTE *imageData, int height, int width, int stride, int nParameter) {
	int i = 0, t = 0;
	double dPercent = static_cast<double>(nParameter) / 100;

	BYTE* imgpt = imageData;

	for (int i = 0; i < height; ++i) {
		imgpt = imageData + i*stride;
		int n = 0;
		for (int j = 0; j < width; ++j) {
			BYTE b = imgpt[n];
			BYTE g = imgpt[n + 1];
			BYTE r = imgpt[n + 2];
		
			GetValue(b, g, r, dPercent);

			imgpt[n] = b;
			imgpt[n + 1] = g;
			imgpt[n + 2] = r;

			n += 3;
		}
	}
	return imageData;
}