#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <process.h>
#include "picture.h"

int Picture::ReadFileHeader(char *filepath, BITMAPFILEHEADER *bmfh) {

	FILE *fp;
	fp = fopen(filepath, "rb");
	if (!fp) {
		printf("Can not open the file:%s\n", filepath);
		return -1;
	}
	if (fread(&bmfh->bfType, sizeof(WORD), 1, fp) != 1) {
		printf("Can not read bfType in the file header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmfh->bfSize, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read bfSize in the file header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmfh->bfReserved1, sizeof(WORD), 1, fp) != 1) {
		printf("Can not read bfReserved1 in the file header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmfh->bfReserved2, sizeof(WORD), 1, fp) != 1) {
		printf("Can not read bfReserved2 in the file header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmfh->bfOffBits, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read bfOffBits in the file header.\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int Picture::ReadInfoHeader(char *filepath, BITMAPINFOHEADER *bmih) {

	FILE *fp;
	fp = fopen(filepath, "rb");
	if (!fp) {
		printf("Can not open the file:%s\n", filepath);
		return -1;
	}
	fseek(fp, 14, SEEK_SET);
	if (fread(&bmih->biSize, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read biSize in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biWidth, sizeof(LONG), 1, fp) != 1) {
		printf("Can not read biWidth in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biHeight, sizeof(LONG), 1, fp) != 1) {
		printf("Can not read biHeight in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biPlanes, sizeof(WORD), 1, fp) != 1) {
		printf("Can not read biPlanes in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biBitCount, sizeof(WORD), 1, fp) != 1) {
		printf("Can not read biBitCount in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biCompression, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read biCompression in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biSizeImage, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read biSizeImage in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biXPelsPerMeter, sizeof(LONG), 1, fp) != 1) {
		printf("Can not read biXPelsPerMeter in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biYPelsPerMeter, sizeof(LONG), 1, fp) != 1) {
		printf("Can not read biYPelsPerMeter in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biClrUsed, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read biClrUsed in the info header.\n");
		fclose(fp);
		return -1;
	}
	if (fread(&bmih->biClrImportant, sizeof(DWORD), 1, fp) != 1) {
		printf("Can not read biClrImportant in the info header.\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int Picture::CreatePalette() {

	int i;
	if (sizeof(pal) / sizeof(RGBQUAD) != 256)
	{
		printf("The size of the palette must be 256.\n");
		return -1;
	}
	for (i = 0; i < 256; i++)
	{
		pal[i].rgbBlue = i;
		pal[i].rgbGreen = i;
		pal[i].rgbRed = i;
		pal[i].rgbReserved = 0;
	}
	return 0;
}

int Picture::ReadPixelData(char *filepath) {

	FILE *fp;
	int n;
	n = ReadFileHeader(filepath, &bmfh);
	if (n == -1) {
		printf("Can not read the file header of the BMP file.\n");
		return -1;
	}
	else {
		bReadBMFH = true;
	}
	n = ReadInfoHeader(filepath, &bmih);
	if (n == -1) {
		printf("Can not read the info header of the BMP file.\n");
		return -1;
	}
	else {
		bReadBMIH = true;
	}

	width = bmih.biWidth;
	height = bmih.biHeight;
	bitCount = bmih.biBitCount;
	dwLineBytes = GetLineBytes();
	stride = dwLineBytes;
	imgData = (BYTE*)malloc(dwLineBytes*height*sizeof(BYTE));

	if (_msize(imgData) != (dwLineBytes*height))
	{
		printf("The size you allocate for the pixel data is not right.\n");
		printf("Fittable size: %ld bytes.\n", (dwLineBytes*height));
		printf("Your size: %ld bytes.\n", sizeof(imgData));
		return -1;
	}
	
	if (!imgData)
	{
		printf("Can not allocate memory for the pixel data.\n");
	}
	fp = fopen(filepath, "rb");
	if (!fp)
	{
		printf("Can not open the file: %s\n", filepath);
		free(imgData);
		return -1;
	}
	if (bitCount == 8)
	{
		fseek(fp, bmfh.bfOffBits, SEEK_SET);
	}
	else if (bitCount == 24)
	{
		fseek(fp, bmfh.bfOffBits, SEEK_SET);
	}
	else
	{
		printf("Only Support: 8 or 24 bits.\n");
		free(imgData);
		fclose(fp);
		return -1;
	}
	if (fread(imgData, dwLineBytes*height*sizeof(BYTE), 1, fp) != 1)
	{
		printf("Can not read the pixel data.\n");
		free(imgData);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

LONG Picture::GetLineBytes() {

	return (width*bitCount + 31) / 32 * 4;
}


int Picture::CheckFilePath(char *filepath) {

	FILE *fp;
	int len = strlen(filepath) / sizeof(char);
	char ext[3];
	if (filepath[0] != int('\"'))
	{
		strncpy(ext, &filepath[len - 3], 3);
		if (!(ext[0] == 'b' && ext[1] == 'm' && ext[2] == 'p'))
		{
			printf("Error: The file is not a BMP file.\n");
			printf("Error: The extention of the filename must be 'bmp',not 'BMP'\n");
			return -1;
		}
		fp = fopen(filepath, "r");
		if (!fp)
		{
			printf("Error: The path is not correct.\n");
			return -1;
		}
		fclose(fp);
	}
	else
	{
		printf("Error: The path must not include blank space.\n");
		return -1;
	}
	return 0;
}

int Picture::SaveAsImage(char *filepath) {

	FILE *fp;
	int i;
	if (!bReadBMFH)
	{
		printf("Error: read the file header at first.\n");
		return -1;
	}
	if (!bReadBMIH)
	{
		printf("Error: read the info header at first.\n");
		return -1;
	}
	if (bmih.biBitCount != 8 && bmih.biBitCount != 24)
	{
		printf("Error: only supported 8 or 24 bits.\n");
		return -1;
	}
	if (!bReadPixel)
	{
		printf("Error: read the pixel data at first.\n");
		return -1;
	}
	fp = fopen(filepath, "wb");
	if (!fp)
	{
		printf("Error: Can not open the file:%s\n", filepath);
		return -1;
	}

	for (i = 0; i < 256; i++)
	{
		pal[i].rgbReserved = 0;
		pal[i].rgbBlue = i;
		pal[i].rgbGreen = i;
		pal[i].rgbRed = i;
	}

	if (fwrite(&bmfh.bfType, sizeof(WORD), 1, fp) != 1)
	{
		printf("Can not write bfType in the file header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmfh.bfSize, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write bfSize in the file header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmfh.bfReserved1, sizeof(WORD), 1, fp) != 1)
	{
		printf("Can not write bfReserved1 in the file header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmfh.bfReserved2, sizeof(WORD), 1, fp) != 1)
	{
		printf("Can not write bfReserved2 in the file header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmfh.bfOffBits, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write bfOffBits in the file header.\n");
		fclose(fp);
		return -1;
	}
	if (fwrite(&bmih.biSize, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write biSize in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biWidth, sizeof(LONG), 1, fp) != 1)
	{
		printf("Can not write biWidth in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biHeight, sizeof(LONG), 1, fp) != 1)
	{
		printf("Can not write biHeight in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biPlanes, sizeof(WORD), 1, fp) != 1)
	{
		printf("Can not write biPlanes in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biBitCount, sizeof(WORD), 1, fp) != 1)
	{
		printf("Can not write biBitCount in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biCompression, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write biCompression in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biSizeImage, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write biSizeImage in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biXPelsPerMeter, sizeof(LONG), 1, fp) != 1)
	{
		printf("Can not write biXPelsPerMeter in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biYPelsPerMeter, sizeof(LONG), 1, fp) != 1)
	{
		printf("Can not write biYPelsPerMeter in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biClrUsed, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write biClrUsed in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (fwrite(&bmih.biClrImportant, sizeof(DWORD), 1, fp) != 1)
	{
		printf("Can not write biClrImportant in the info header.\n");
		fclose(fp);
		return -1;
	}

	if (bmih.biBitCount == 8)
	{
		if (fwrite(pal, sizeof(RGBQUAD), 256, fp) != 256)
		{
			printf("Error: can not write the color palette.\n");
			fclose(fp);
			return -1;
		}
	}

	if (fwrite(imgData, height*dwLineBytes, 1, fp) != 1)
	{
		printf("Error: can not write the pixel data.\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	printf("Save As the image successfully.\n");
	return 0;
}

void Picture::GetGrayData() {

	grayData = (int *)malloc(height*width*sizeof(int));
	BYTE *imgpt = imgData;
	int k = 0;
	for (int i = 0; i < height; ++i) {
		imgpt = imgData + i * stride;
		int n = 0;
		for (int j = 0; j < width; ++j) {
			grayData[k] = (imgpt[n]*0.299 + imgpt[n + 1]*0.587 + imgpt[n + 2]*0.114);
			k += 1;
			n += 3;
		}
	}
}

void Picture::GetClarity() {

	double sum = 0;
	int k = 0;

	/*SMD2 */
	//for (int i = 0; i < height; ++i) {
	//	for (int j = 0; j < width; ++j) {
	//		int m = (((k+1) % width) == 0 ? k : k + 1);
	//		int n = ((k + width) > height*width-1) ? k : k + width;
	//		sum += (double)(abs(grayData[k] - grayData[m])*abs(grayData[k] - grayData[n]));
	//		k += 1;
	//	}
	//}

	/*Brenner */
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int n = ((k + width*2) > height*width - 1) ? k : k + width*2;
			sum += (double)(grayData[k] - grayData[n])*(grayData[k] - grayData[n]);
			k += 1;
		}
	}
	clarity = sum / (height*width);
}

void Picture::GetBright() {

	double sum = 0;
	int Hist[256];
	for (int i = 0; i < 256; ++i) {
		Hist[i] = 0;
	}
	int k = 0;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			sum += (double)(grayData[k] - 128);
			Hist[(int)grayData[k]]++;
			k++;
		}
	}
	brightmean = sum / (height*width*1.0);
	double Ma = 0;
	for (int i = 0; i < 256; ++i) {
		double tmp = i - 128 - brightmean;
		Ma += (tmp > 0 ? tmp : -tmp )*Hist[i];
	}
	Ma = Ma / (height*width*1.0);
	brightvar = (brightmean > 0 ? brightmean : -brightmean) / (Ma > 0 ? Ma : -Ma);

}

void Picture::GetColor() {

}

void Picture::Read(char *filepath) {

	int result = ReadPixelData(filepath);
	if (result != -1) {
		bReadPixel = true;
		//printf("Read image successfully!");
	}
	GetGrayData();
	GetClarity();
	GetBright();
	GetColor();
}

void Picture::Write(char *filepath) {

	int result = SaveAsImage(filepath);
	if (result != -1) {
		printf("Save image successfully!");
	}
}
