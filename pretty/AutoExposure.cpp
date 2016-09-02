#include "AutoExposure.h"
#define MAX_SIZE 5
#define PI 3.14159
#define T_LOW 65
#define T_HIGH 80

void resize(unsigned char* srcData, unsigned char* rSrcData, float* rgrayData, int width, int height, int rwidth, int rheight, int stride)  {
	double xRatio = (double)width / rwidth;
	double yRatio = (double)height / rheight;

	unsigned char* pSrc = srcData;
	unsigned char* prSrc = rSrcData;
	float* prGray = rgrayData;

	for (int i = 0; i < rheight; i++) {

		double srcY = i * yRatio;//源图像“虚”坐标的y值
		int IntY = (int)srcY;//向下取整
		double v = srcY - IntY;//获取小数部分
		double v1 = 1.0 - v;

		for (int j = 0; j < rwidth; j++) {

			double srcX = j * xRatio;//源图像“虚”坐标的x值
			int IntX = (int)srcX;//向下取整
			double u = srcX - IntX;//获取小数部分
			double u1 = 1.0 - u;

			int Index00 = IntY * stride + IntX * 3;//得到原图左上角的像素位置
			int Index10;                            //原图左下角的像素位置
			if (IntY < height - 1) {
				Index10 = Index00 + stride;
			}
			else {
				Index10 = Index00;
			}
			int Index01;                            //原图右上角的像素位置
			int Index11;                            //原图右下角的像素位置
			if (IntX < width - 1) {
				Index01 = Index00 + 3;
				Index11 = Index10 + 3;
			}
			else {
				Index01 = Index00;
				Index11 = Index10;
			}
			double temp0 = (v1 * (u * pSrc[Index01] + u1 * pSrc[Index00]) +
				v * (u * pSrc[Index11] + u1 * pSrc[Index10]));

			double temp1 = (v1 * (u * pSrc[Index01 + 1] + u1 * pSrc[Index00 + 1]) +
				v * (u * pSrc[Index11 + 1] + u1 * pSrc[Index10 + 1]));

			double temp2 = (v1 * (u * pSrc[Index01 + 2] + u1 * pSrc[Index00 + 2]) +
				v * (u * pSrc[Index11 + 2] + u1 * pSrc[Index10 + 2]));

			prGray[0] = temp0 / 255.0*0.114 + temp1 / 255.0*0.587 + temp2 / 255.0*0.299;

			prSrc[0] = (unsigned char)temp0;
			prSrc[1] = (unsigned char)temp1;
			prSrc[2] = (unsigned char)temp2;

			prGray += 1;
			prSrc += 3;
		}
	}
}

void Segmentation(unsigned char* rSrcData, unsigned char* segSrcData, int rwidth, int rheight, int& num_ccs) {
	
	//unsigned char -> image<rgb>
	unsigned char* prSrc = rSrcData;
	image<rgb> *im = new image<rgb>(rwidth, rheight);
	image<rgb>* pim = im;
	int i, j;
	for (j = 0; j < rheight; ++j) {
		for (i = 0; i < rwidth; ++i) {
			rgb tmp;
			tmp.b = prSrc[0];
			tmp.g = prSrc[1];
			tmp.r = prSrc[2];
			imRef(pim, i, j) = tmp;

			prSrc += 3;
		}
	}
	float sigma = 0.25;
	int k = 200;
	int min_size = 15;

	//segmentation
	/*
	  im: 转化后的image<rgb>
	  flagData:每个像素的标签
	  rgrayData:缩小后的灰度图
	*/
	//image<rgb> *im, float sigma, float c, int min_size, int *num_ccs
	image<rgb>* seg = segment_image(im, sigma, k, min_size, &num_ccs);

	pim = seg;
	//image<rgb> -> unsigned char
	unsigned char* pseg = segSrcData;
	for (j = 0; j < rheight; ++j) {
		for (i = 0; i < rwidth; ++i) {
			pseg[0] = imRef(pim, i, j).b;
			pseg[1] = imRef(pim, i, j).g;
			pseg[2] = imRef(pim, i, j).r;
			pseg += 3;
		}
	}

	//delete im;
	//delete[] im;
	im = NULL;
}

//void showgrayu(unsigned char*grayData, int width, int height) {
//	Mat result(height, width, CV_8UC1);
//	unsigned char* tmp = grayData;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			result.at<uchar>(i, j) = tmp[0];
//			tmp += 1;
//		}
//	}
//	imwrite("F:/img2/resultgray.jpg", result);
//}

//void showone(float* data, int width, int height) {
//	Mat result(height, width, CV_8UC1);
//	float* pdata = data;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			result.at<uchar>(i, j) = (uchar)(pdata[0] * 255);
//			pdata++;
//		}
//	}
//	imwrite("F:/img2/resultone.jpg", result);
//}

//void showthree(float* data, int width, int height) {
//	Mat result(height, width, CV_8UC3);
//	float* pdata = data;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			result.at<Vec3b>(i, j)[0] = (uchar)(pdata[0] * 255);
//			result.at<Vec3b>(i, j)[1] = (uchar)(pdata[1] * 255);
//			result.at<Vec3b>(i, j)[2] = (uchar)(pdata[2] * 255);
//			pdata += CHANNEL;
//		}
//	}
//	imwrite("F:/img2/resultthree.jpg", result);
//}

void GammaDetail(float* rgrayData, float* rgrayg1Data, float* rgrayg2Data, float* flagData, 
	float* vs, float* vh, float* allnum, float gamma1, float gamma2, int rwidth, int rheight) {
	
	//gamma1 = 2.2 gamma2 = 0.455

	float* prgray = rgrayData;
	float* prgrayg1 = rgrayg1Data;
	float* prgrayg2 = rgrayg2Data;

	unsigned char* rgrayD = new unsigned char[rwidth*rheight];
	unsigned char* rgrayg1D = new unsigned char[rwidth*rheight];
	unsigned char* rgrayg2D = new unsigned char[rwidth*rheight];

	unsigned char* prgrayD = rgrayD;
	unsigned char* prgrayg1D = rgrayg1D;
	unsigned char* prgrayg2D = rgrayg2D;

	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			prgrayg1[0] = pow(prgray[0], gamma1);//r
			prgrayg2[0] = pow(prgray[0], gamma2);//r-1

			prgrayD[0] = (unsigned char)(prgray[0]*255);
			prgrayg1D[0] = (unsigned char)(prgrayg1[0]*255);
			prgrayg2D[0] = (unsigned char)(prgrayg2[0]*255);

			prgray++;
			prgrayg1++;
			prgrayg2++;

			prgrayD++;
			prgrayg1D++;
			prgrayg2D++;
		}
	}
	rgrayD = canny(rgrayD, rwidth, rheight);	
	rgrayg1D = canny(rgrayg1D, rwidth, rheight);	
	rgrayg2D = canny(rgrayg2D, rwidth, rheight);
	
	//showgrayu(rgrayg1D, rwidth, rheight);

	prgrayD = rgrayD;
	prgrayg1D = rgrayg1D;
	prgrayg2D = rgrayg2D;
	float* pflag = flagData;

	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			int tmp = prgrayg1D[0] & prgrayg2D[0];//两个gamma校正图像边缘交
			int all = prgrayD[0] + prgrayg1D[0] + prgrayg2D[0];//三个图像边缘并
			all = (all >= 255) ? 255 : 0;

			prgrayg1D[0] = prgrayg1D[0] - tmp;//h
			prgrayg2D[0] = prgrayg2D[0] - tmp;//s

			int order = pflag[0] * 10;//求取每一个像素的灰度等级

			if (all == 255) {
				allnum[order] += 1.0;
			}
			if (prgrayg1D[0] == 255) {
				vs[order] += 1.0;
			}
			if (prgrayg2D[0] == 255) {
				vh[order] += 1.0;
			}
			prgrayD++;
			prgrayg1D++;
			prgrayg2D++;
			pflag++;
		}
	}

	for (int i = 0; i < 11; ++i) {
		if (allnum[i] != 0) {
			vs[i] = vs[i] / allnum[i];
		}
		else {
			vs[i] = 0.0;
		}
		if (allnum[i] != 0) {
			vh[i] = vh[i] / allnum[i];
		}
		else {
			vh[i] = 0.0;
		}
	}	
	delete[] rgrayD;
	delete[] rgrayg1D;
	delete[] rgrayg2D;
}

//void show(unsigned char* srcData, int width, int height) {
//
//	Mat result(height, width, CV_8UC3);
//	unsigned char* psrc = srcData;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			result.at<Vec3b>(i, j)[0] = psrc[0];
//			result.at<Vec3b>(i, j)[1] = psrc[1];
//			result.at<Vec3b>(i, j)[2] = psrc[2];
//			psrc += CHANNEL;
//		}
//	}
//	imwrite("F:/img2/result.jpg", result);
//}
//
//void showgray(float* grayData, int width, int height) {
//
//	Mat result(height, width, CV_8UC1);
//	float* pgray = grayData;
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			result.at<uchar>(i, j) = (uchar)(pgray[0] * 255);
//			pgray += 1;
//		}
//	}
//	imwrite("F:/img2/resultgray.jpg", result);
//}

float CaculateEi(int a0, int a1, int a2, int a3, int a4, int a5, int a6,
	int a7, int a8, int a9, int a10, float* vs, float* vh, float* csize, int* flaggray) {

	float Pi = 0, Ei = 0;
	float arr[11][2];

	arr[0][0] = (float)a0 / 10; arr[0][1] = 0.0;
	arr[1][0] = (float)a1 / 10; arr[1][1] = 0.1;
	arr[2][0] = (float)a2 / 10; arr[2][1] = 0.2;
	arr[3][0] = (float)a3 / 10; arr[3][1] = 0.3;
	arr[4][0] = (float)a4 / 10; arr[4][1] = 0.4;
	arr[5][0] = (float)a5 / 10; arr[5][1] = 0.5;
	arr[6][0] = (float)a6 / 10; arr[6][1] = 0.6;
	arr[7][0] = (float)a7 / 10; arr[7][1] = 0.7;
	arr[8][0] = (float)a8 / 10; arr[8][1] = 0.8;
	arr[9][0] = (float)a9 / 10; arr[9][1] = 0.9;
	arr[10][0] = (float)a10 / 10; arr[10][1] = 1.0;

	for (int i = 0; i < 11; i++) {
		if (i < 5 && flaggray[i] == 1) {
			Pi = vs[i] * csize[i] * (1 / (1 + exp(-(arr[i][0] - arr[i][1]))));
			if (Pi != 0) {
				Ei += -log(Pi);
			}			
		}
		else if (i > 5 && flaggray[i] == 1) {
			Pi = vh[i] * csize[i] * (1 / (1 + exp(-(arr[i][1] - arr[i][0]))));
			if (Pi != 0) {
				Ei += -log(Pi);
			}			
		}
	}
	return Ei;
}

float CaculatePij(int a0, int a1, int a2, int a3, int a4, int a5, int a6,
	int a7, int a8, int a9, int a10, int** neighbor, float* csize) {
	float Pij = 0, Eij = 0;
	float arr[11][2];
	float E = 2.718, X = 3.14;

	arr[0][0] = (float)a0 / 10; arr[0][1] = 0.0;
	arr[1][0] = (float)a1 / 10; arr[1][1] = 0.1;
	arr[2][0] = (float)a2 / 10; arr[2][1] = 0.2;
	arr[3][0] = (float)a3 / 10; arr[3][1] = 0.3;
	arr[4][0] = (float)a4 / 10; arr[4][1] = 0.4;
	arr[5][0] = (float)a5 / 10; arr[5][1] = 0.5;
	arr[6][0] = (float)a6 / 10; arr[6][1] = 0.6;
	arr[7][0] = (float)a7 / 10; arr[7][1] = 0.7;
	arr[8][0] = (float)a8 / 10; arr[8][1] = 0.8;
	arr[9][0] = (float)a9 / 10; arr[9][1] = 0.9;
	arr[10][0] = (float)a10 / 10; arr[10][1] = 1.0;

	float t1 = sqrt(2 * X * 0.15 * 0.15);
	float t2 = 2 * 0.15 * 0.15;

	for (int i = 0; i < 11; ++i) {
		for (int j = i+1; j < 11; ++j) {
			if (neighbor[i][j] == 1) {
				int newDij = abs(arr[i][0] - arr[j][0]);
				int Dijdist = newDij - abs(arr[i][1] - arr[j][1]);
				float t3 = Dijdist*Dijdist;
				Pij = csize[j] * pow(E, -t3 / t2) / t1;//csize:Cj
				if (Pij != 0) {
					Eij += -log(Pij);
				}				
			}
		}
	}

	return Eij;
}
//flagData, rwidth, rheight, neighbor);
//判断四个邻域
void GetNeighbor(float* flagData, int rwidth, int rheight, int** neighbor) {

	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			int inorder = (int)(flagData[j*rwidth + i] * 10);//获得当前像素的灰度级编号
			if ( ((j - 1)*rwidth + i) >= 0 && ((j - 1)*rwidth + i) < rheight*rwidth) {
				int norder = (int)(flagData[(j - 1)*rwidth + i]*10);
				if (neighbor[inorder][norder] != 1) {
					neighbor[inorder][norder] = 1;
				}				
			}
			if ((j *rwidth + i-1) >= 0 && (j*rwidth + i-1) < rheight*rwidth) {
				int norder = (int)(flagData[j *rwidth + i-1]*10);
				if (neighbor[inorder][norder] != 1) {
					neighbor[inorder][norder] = 1;
				}
			}
			if (((j + 1)*rwidth + i) >= 0 && ((j + 1)*rwidth + i) < rheight*rwidth) {
				int norder = (int)(flagData[(j + 1)*rwidth + i]*10);
				if (neighbor[inorder][norder] != 1) {
					neighbor[inorder][norder] = 1;
				}
			}
			if ((j *rwidth + i+1) >= 0 && (j *rwidth + i+1) < rheight*rwidth) {
				int norder = (int)(flagData[j *rwidth + i+1]*10);
				if (neighbor[inorder][norder] != 1) {
					neighbor[inorder][norder] = 1;
				}
			}			
		}
	}
}

float argMinCalculate(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10,
	float* vs, float* vh, float* csize, int* flaggray, int** neighbor, float lamda) {

	float sum = 0;
	float Ei = 0;
	float Pi = 0;

	Ei = CaculateEi(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, vs, vh, csize, flaggray);
	Pi = CaculatePij(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, neighbor, csize);

	sum = Ei + lamda*Pi;
	return sum;
}

int* FindMaxForOne(int* p, int* q) {
	int* p1 = p;
	int* p2 = q;
	while (*p2 <= *p1) {
		p2--;
	}
	return p2;
}

void Reverse(int* start, int* end) {
	while (start < end) {
		int tmp = *start;
		*start = *end;
		*end = tmp;
		start++;
		end--;
	}
}

//求规则的问题
void calculateZ(float* vs, float* vh, float* csize, int* flaggray, int** neighbor, int* regionzone) {//设每一个zone相对之前的灰度不超过上下3个等级

	float minvalue = 100000.0;
	int* a = new int[11]();
	for (int i = 0; i < 11; ++i) {
		a[i] = i;
	}
	int* newzone = regionzone;
	float lamda;
	float Eisum = 0, Pisum = 0;
	//-------------求lamda--------------
	while (true) {
		int k;
		for (k = 10; k >= 0; --k) {
			if (a[5] == 5) {
				Eisum += CaculateEi(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], vs, vh, csize, flaggray);
				Pisum += CaculatePij(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], neighbor, csize);
			}
			if (k == 5) {
				continue;
			}
			if (k > 5) {
				int high = 9;
				int low = 6;
				if (a[k] < high) {
					a[k]++;
					break;
				}
				else {
					a[k] = low;
					k--;
				}
			}
			else if (k < 5) {
				int high = 4;
				int low = 1;
				if (a[k] < high) {
					a[k]++;
					break;
				}
				else {
					a[k] = low;
					k--;
				}
			}
		}
		if (k < 0) {
			break;
		}
	}
	
	lamda = Eisum / Pisum;
	//---------------------------------
	while (true) {
		int k;
		if (a[5] == 5) {
			float tmp = argMinCalculate(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
				vs, vh, csize, flaggray, neighbor, lamda);
			if (tmp < minvalue) {
				minvalue = tmp;
				for (int i = 0; i < 11; ++i) {
					newzone[i] = a[i];
				}
			}
		}
		
		for (k = 10; k >= 0; --k) {
			if (k == 5) {
				continue;
			}
			if (k > 5 && k < 11) {
				int high = 9;
				int low = 6;
				if (a[k] < high) {
					a[k]++;
					break;
				}
				else {
					a[k] = low;
					k--;
				}
			}
			else if (k > 0 && k < 4) {
				int high = 4;
				int low = 1;
				if (a[k] < high) {
					a[k]++;
					break;
				}
				else {
					a[k] = low;
					k--;
				}
			}
		}
		if (k < 0) {
			break;
		}
	}
	delete[] a;
}

void calculateQsQh(float* rgrayData, float* flagData, int* flaggray, int* regionzone, float* csize, float& Qs, float& Qh, int rwidth, int rheight) {
	
	//每一个区域，都要求取该区域的所有像素均值以及f(x)
	//float* meanData = new float[11]();
	//float* fxData = new float[11]();
	//float* fxDataf = new float[11]();
	//int* regionsum = new int[11]();

	//float* pflag = flagData;
	//float* prgray = rgrayData;

	//for (int j = 0; j < rheight; ++j) {
	//	for (int i = 0; i < rwidth; ++i) {

	//		int order = (int)(pflag[0] * 10);
	//		meanData[order] += prgray[0];//各个区域的均值
	//		regionsum[order] += 1;

	//		float tmpcolor = prgray[0];
	//		fxData[order] += 5 * tmpcolor*exp(-14 * pow(tmpcolor, 1.6));
	//		tmpcolor = 1 - prgray[0];
	//		fxDataf[order] += 5 * tmpcolor*exp(-14 * pow(tmpcolor, 1.6));

	//		pflag += 1;
	//		prgray += 1;
	//	}
	//}
	//int ssum = 0, hsum = 0;
	//for (int i = 0; i < 11; ++i) {
	//	if (flaggray[i] == 1) {
	//		meanData[i] = meanData[i] / regionsum[i];
	//		if (i < 5) {
	//			if (fxData[i]) {
	//				Qs = (regionzone[i] * 1.0 / 10.0 - meanData[i])*regionsum[i] / fxData[i];
	//				Qs += Qs*regionsum[i];
	//				ssum += regionsum[i];
	//			}			
	//		}
	//		else if (i > 5) {
	//			if (fxDataf[i]) {
	//				Qh = (meanData[i] - regionzone[i] * 1.0 / 10.0)*regionsum[i] / fxDataf[i];
	//				Qh += Qh*regionsum[i];
	//				hsum += regionsum[i];
	//			}			
	//		}
	//	}		
	//}
	//Qs = Qs / ssum;
	//Qh = Qh / hsum;
	//printf("%f ", Qs);
	//printf("%f", Qh);

	//delete[] meanData;
	//delete[] fxData;
	//delete[] fxDataf;

	double e_old = 0, e_new = 0, f_val = 0;
	int i, j;

	e_old = 0.1 + 0.2 + 0.3 + 0.4;
	e_new = (double)regionzone[0] / 10 + (double)regionzone[1] / 10 + (double)regionzone[2] / 10//新的灰度等级
		+ (double)regionzone[3] / 10 + (double)regionzone[4] / 10;

	for (i = 0; i < 5; i++){
		double tmp = (double)i / 10;
		f_val += 5 * tmp * exp(-14 * pow(tmp, 1.6));
	}

	Qs = (e_new - e_old) * (csize[0] + csize[1] + csize[2] + csize[3] + csize[4]) * f_val;

	e_old = 0.6 + 0.7 + 0.8 + 0.9 + 1.0;
	e_new = (double)regionzone[6] / 10 + (double)regionzone[7] / 10 + (double)regionzone[8] / 10
		+ (double)regionzone[9] / 10 + (double)regionzone[10] / 10;

	for (i = 6; i < 11; i++){
		double tmp = (double)i / 10;
		f_val += 5 * tmp * exp(-14 * pow(tmp, 1.6));
	}

	Qh = (e_new - e_old) * (csize[6] + csize[7] + csize[8] + csize[9] + csize[10]) * f_val;

	Qs = Qs / 2;
	Qh = Qh / 10;
}

void S_curve(float* imgnorm, float* gray, float Qs, float Qh, int width, int height) {
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			float f_vals = 5 * gray[0] * exp(-14 * pow(gray[0], 1.6));//f_三角x
			float f_valh = 5 * gray[0] * exp(-14 * pow(1.0 - gray[0], 1.6));
			//printf("%f %f", f_vals, f_valh);

			float newgray = gray[0] + Qs * f_vals - Qh * f_valh;
			if (newgray > 1.0) {
				newgray = 0.99;
			}
			if (newgray < 0.0) {
				newgray = 0.01;
			}
			float scale = newgray / gray[0];

			gray[0] = newgray;

			float Uvalue = -0.147*imgnorm[0] - 0.289*imgnorm[1] + 0.436*imgnorm[2];
			float Vvalue = 0.615*imgnorm[0] - 0.515*imgnorm[1] - 0.1*imgnorm[2];

			Uvalue = Uvalue*scale;
			Vvalue = Vvalue*scale;

			imgnorm[0] = gray[0] + 1.140*Vvalue;
			imgnorm[0] = imgnorm[0] > 1.0 ? 0.999 : imgnorm[0];
			imgnorm[0] = imgnorm[0] < 0.0 ? 0.001 : imgnorm[0];

			imgnorm[1] = gray[0] - 0.394*Uvalue - 0.581*Vvalue;
			imgnorm[1] = imgnorm[1] > 1.0 ? 0.999 : imgnorm[1];
			imgnorm[1] = imgnorm[1] < 0.0 ? 0.001 : imgnorm[1];

			imgnorm[2] = gray[0] + 2.032*Uvalue;
			imgnorm[2] = imgnorm[2] > 1.0 ? 0.999 : imgnorm[2];
			imgnorm[2] = imgnorm[2] < 0.0 ? 0.001 : imgnorm[2];

			imgnorm += 3;
			gray += 1;
		}
	}
}

void GetResult(float* imgnorm, unsigned char* srcData, int width, int height, int stride) {
	float* pnorm = imgnorm;
	unsigned char* psrc = srcData;
	int offset = stride - width * CHANNEL;

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			psrc[0] = pnorm[0] * 255;
			psrc[1] = pnorm[1] * 255;
			psrc[2] = pnorm[2] * 255;

			psrc[0] = (psrc[0] > 255) ? 255 : psrc[0];
			psrc[1] = (psrc[1] > 255) ? 255 : psrc[1];
			psrc[2] = (psrc[2] > 255) ? 255 : psrc[2];

			psrc[0] = (psrc[0] < 0) ? 0 : psrc[0];
			psrc[1] = (psrc[1] < 0) ? 0 : psrc[1];
			psrc[2] = (psrc[2] < 0) ? 0 : psrc[2];

			psrc += 3;
			pnorm += 3;
		}
		psrc += offset;
	}
}

void Combine(unsigned char*segSrcData, float* rgrayData, float* flagData, int rwidth, int rheight, int num_ccs, float* csize, int* flaggray) {

	float* graysum = new float[1200]();
	int* regionnum = new int[1200]();
	unsigned char* pseg = segSrcData;
	float* prgray = rgrayData;
	float* sizesum = new float[12]();

	//统计相同颜色的区域的总像素和以及区域数量
	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			int color = (int)(pseg[0] / 255.0 * 1000 + pseg[1] / 255.0 * 100 + pseg[2] / 255.0 * 10);
			graysum[color] += prgray[0];
			regionnum[color] += 1;
			pseg += 3;
			prgray += 1;
		}
	}
	//graysum为区域i的平均颜色
	for (int i = 0; i < 1200; ++i) {
		if (regionnum[i] != 0) {
			graysum[i] = graysum[i] / regionnum[i];
			graysum[i] = (round(graysum[i] * 10)*1.0 + 0.001) / 10.0;
		}
	}

	pseg = segSrcData;
	float* pflag = flagData;
	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			int color = (int)(pseg[0] / 255.0 * 1000 + pseg[1] / 255.0 * 100 + pseg[2] / 255.0*10);
			float tmp = graysum[color];
			pflag[0] = tmp;

			pseg += 3;
			pflag += 1;
		}
	}
	//showgray(flagData, rwidth, rheight);

	pflag = flagData;
	for (int j = 0; j < rheight; ++j) {
		for (int i = 0; i < rwidth; ++i) {
			float tmp = pflag[0];
			int graylevel = (int)(tmp * 10);
			sizesum[graylevel] += 1;
			pflag++;
		}
	}
	int sum = 0;
	for (int i = 0; i < 11; ++i) {
		if (sizesum[i] > 0) {
			flaggray[i] = 1;
			csize[i] = sizesum[i]*1.0 / (rheight*rwidth);
			sum += sizesum[i];
		}
	}
	delete[] graysum;
	delete[] regionnum;
	delete[] sizesum;
	//showgray(rgrayData, rwidth, rheight);
}

unsigned char* AutoExposure(unsigned char* srcData, int height, int width, int stride) {

	int offset = stride - width * CHANNEL;
	unsigned char* pSrc = srcData;
	
	float* gray = new float[height*width];//gray image
	float* imgnorm = new float[height*width*3];//normalization
	float* pgray = gray;
	float* pnorm = imgnorm;
	int i, j, num_ccs;

	int ratio = 30;//scale ratio
	int rheight = height / ratio, rwidth = width / ratio;	

	unsigned char* rSrcData = new unsigned char[rheight*rwidth*3];
	unsigned char* segSrcData = new unsigned char[rheight*rwidth*3];
	float* rgrayData = new float[rheight*rwidth];
	float* flagData = new float[rheight*rwidth];//每个像素是什么灰度等级
	float* rgrayg1Data = new float[rheight*rwidth];
	float* rgrayg2Data = new float[rheight*rwidth];
	float gamma1 = 2.2, gamma2 = 0.455;

	float* vs = new float[11]();
	float* vh = new float[11]();
	float* allnum = new float[11]();
	float* csize = new float[11]();
	int* flaggray = new int[11]();//有哪些灰度等级是有像素的,如果该灰度等级有像素，则为1
	float Qs = 0, Qh = 0;

	int* regionzone = new int[11]();

	int** neighbor;//判断相邻：11*11
	neighbor = new int *[11]();
	for (int i = 0; i < 11; i++) {
		neighbor[i] = new int[11]();
	}

	//gray and normalize
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {

			pnorm[0] = (pSrc[0] + 0.5) / 256.0;
			pnorm[1] = (pSrc[1] + 0.5) / 256.0;
			pnorm[2] = (pSrc[2] + 0.5) / 256.0;

			pgray[0] = pnorm[0] * 0.114 + pnorm[1] * 0.587 + pnorm[2] * 0.299;

			pSrc += CHANNEL;
			pnorm += 3;
			pgray += 1;
		}
		pSrc += offset;
	}

	
	/*
	  对图像进行缩小操作：
	  srcData:原图
	  rSrcData:缩小后的彩图
	  rgrayData:缩小后的灰度图
	  rwidth rheight:分别是缩小后的宽和高
	*/
	resize(srcData, rSrcData, rgrayData, width, height, rwidth, rheight, stride);
	//show(rSrcData, rwidth, rheight);
	//showone(rgrayData, rwidth, rheight);
	/*
	  对缩小后的彩图进行分割：
	  rSrcData:缩小后的彩图
	  segSrcData:分割后的图
	  flagData:标记有哪些灰度等级是有数据的
	  rgrayData:缩小后的灰度图
	  num_ccs:分割区域数量
	*/
	Segmentation(rSrcData, segSrcData, rwidth, rheight, num_ccs);
	//show(segSrcData, rwidth, rheight);

	/*
	  对分割的区域进行合并，灰度等级相同的合并
	  segSrcData:分割后的彩色标记图
	  rgrayData:缩小后的灰度图
	  flagData:给每一个像素它所属的灰度标记
	  csize:每个区域大小占整个图像大小的比例
	  segSrcData， rgrayData未变,flagData为输出
	*/
	Combine(segSrcData, rgrayData, flagData, rwidth, rheight, num_ccs, csize, flaggray);
	//showone(flagData, rwidth, rheight);

	//Gamma curve
	/*
	  rgrayData:缩小后的灰度图
	  rgrayg1Data:经过gamma1校正后的灰度图
	  rgrayg2Data:经过gamma2校正后的灰度图
	  flagData:每个像素的灰度等级值 0-1
	  flaggray:共占有了多少个灰度等级，若占用，则相应标记位为1
	  vs:各灰度级的vs占比
	  vh:各灰度级的vh占比
	*/
	GammaDetail(rgrayData, rgrayg1Data, rgrayg2Data, flagData, vs, vh, allnum, 
		gamma1, gamma2, rwidth, rheight);

	/*
	  获取灰度等级i和j区域是否相邻数据，若相邻neighbor[i][j]=1
	  flagData:每个像素的灰度等级值 0-1
	*/
	GetNeighbor(flagData, rwidth, rheight, neighbor);
	//for (int i = 0; i < 11; ++i) {
	//	for (int j = 0; j < 11; ++j) {
	//		cout << neighbor[i][j] << " ";
	//	}
	//	cout << endl;
	//}

	/*
	  计算校正后的灰度等级
	  regionzone: 保存每个区域的合适的灰度等级
	*/
	calculateZ(vs, vh, csize, flaggray, neighbor, regionzone);


	calculateQsQh(rgrayData, flagData, flaggray, regionzone, csize, Qs, Qh, rwidth, rheight);


	/*cout << "Qs" << Qs << endl;
	cout << "Qh" << Qh << endl;*/

	S_curve(imgnorm, gray, Qs, Qh, width, height);
	
	GetResult(imgnorm, srcData, width, height, stride);

	delete[] gray;
	delete[] imgnorm;
	delete[] rSrcData;
	delete[] segSrcData;
	delete[] rgrayData;
	delete[] flagData;
	delete[] rgrayg1Data;
	delete[] rgrayg2Data;
	delete[] vs;
	delete[] vh;
	delete[] allnum;
	delete[] csize;
	delete[] flaggray;
	delete[] regionzone;
	delete[] neighbor;

	return srcData;
}