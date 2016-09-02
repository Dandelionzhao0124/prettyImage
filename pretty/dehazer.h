#ifndef DEHAZER_H
#define DEHAZER_H

#include <math.h>

class Dehazer {

public:
	/*Function:Fog removing from the image.
	* function name: Dahazing
	* input param: imgData - initial image data.
	* input param: width - image width
	* input param: height - image height
	* input param: stride - image stride
	* input param: clarity - image clarity evaluation
	* input param: brightmean - image brightness mean value
	* input param: brightvar - image brightness var value
	* output param: the fog removing result of the image
	* steps:
	*    1、input image
	*    2、the dark channel of the three channels: MxData
	*    3、Get the mean filter of the dark channels: BoxblurData
	*    4、the mean value of the dark channels: meanvalue
	*    5、Get LxData: L(x) = min(min(rol*meanvalue, 0.9)*BoxblurData, MxData);
	*    6、Get the Avalue: A = 1/2*(max(max(imgData))+max(BoxblurData))
	*    7、FxData = (imgData-LxData) / (1-(LxData / Avalue))
	*/
	unsigned char* Dehazing(unsigned char* imgData, int height, int width, int stride, float mean, float var);
	~Dehazer() {
		if (MxData != NULL) {
			delete[] MxData;
			MxData = NULL;
		}
		if (BoxblurData != NULL) {
			delete[] BoxblurData;
			BoxblurData = NULL;
		}
		if (LxData != NULL) {
			delete[] LxData;
			LxData = NULL;
		}
		if (Table != NULL) {
			delete[] Table;
			Table = NULL;
		}
	}
private:
	float meanvalue;
	float Avalue;

	unsigned char *grayData;
	unsigned char *MxData;
	unsigned char *BoxblurData;
	unsigned char *LxData;
	unsigned char *Table;

	int height, width, stride;
	float brightmean, brightvar;
	int radius;
	float rol;
	int beta;

	void init(unsigned char* imageData, int height, int width, int stride, float mean, float var);
	void MinValue(unsigned char* imageData, unsigned char* MxData, int height, int width, int stride, float& meanvalue);
	void blur(unsigned char* imageData, int height, int width, int stride);
	void BoxBlur(unsigned char* MxData, unsigned char* BoxblurData, int height, int width, int stride, unsigned char* imageData);
	void GetLxData(unsigned char* MxData, unsigned char* LxData, int height, int width, int stride, unsigned char* imageData);
	void GetAvalue(unsigned char* imageData, unsigned char* BoxblurData, float& Avalue, int height, int width, int stride);
	void GetFxData(unsigned char* imageData, unsigned char* LxData, unsigned char* Table, int height, int width, int stride);
};

#endif