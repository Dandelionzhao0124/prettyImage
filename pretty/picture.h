#ifndef PICTURE_H
#define PICTURE_H

//typedef new struct 
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER
{ // bmfh
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
}BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER
{ // bmih
	DWORD biSize;
	LONG   biWidth;
	LONG   biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG   biXPelsPerMeter;
	LONG   biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}BITMAPINFOHEADER;
typedef struct tagRGBQUAD
{ // rgbq
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
}RGBQUAD;
typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
}BITMAPINFO;

struct Picture {

	//bmp image include
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	bool bReadBMFH = false;
	bool bReadBMIH = false;
	bool bReadPixel = false;

	int bitCount;
	RGBQUAD pal[256];
	DWORD dwLineBytes;

	//private function
	int CheckFilePath(char *filepath);
	int ReadFileHeader(char *filepath, BITMAPFILEHEADER *bmfh);
	int ReadInfoHeader(char *filepath, BITMAPINFOHEADER *bmih);
	int CreatePalette();
	
	LONG GetLineBytes();
	int ReadPixelData(char *filepath);
	int SaveAsImage(char *filepath);

	BYTE *imgData;
	int *grayData;
	int height, width, stride;
	double clarity, brightmean, brightvar;

	void Read(char *filepath);
	void Write(char *filepath);
	void GetGrayData();
	void GetClarity();
	void GetBright();
	void GetColor();
};
#endif