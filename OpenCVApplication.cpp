#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <fstream>
#include <random>
#include <float.h>
#include <queue>
#include <random>

using namespace std;
//using namespace cv;

int isInside(Mat img, int i, int j) {
	return i >= 0 && i < img.rows && j >= 0 && j < img.cols;
}

float max3(float a, float b, float c) {
	float max = a;
	if (b > max) {
		max = b;
	}
	if (c > max) {
		max = c;
	}
	return max;
}

float min3(float a, float b, float c) {
	float min = a;
	if (b < min)
	{
		min = b;
	}
	if (c < min) {
		min = c;
	}

	return min;
}

Mat_<Vec3b> RGB_to_HSV(Mat_<Vec3b> img) {
	Mat_<Vec3b> img1(img.rows, img.cols);
	for (int i = 0; i < img.rows;i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			float red = img(i, j)[2] / 255.0;
			float green = img(i, j)[1] / 255.0;
			float blue = img(i, j)[0] / 255.0;

			float M = max3(red, green, blue);
			float m = min3(red, green, blue);

			float C = M - m;

			//V
			float V = M;

			//S
			float S;
			if (V != 0) {
				S = C / V;
			}
			else {	//negru
				S = 0;
			}

			//H
			float H;
			if (C != 0) {
				if (M == red) {
					H = 60 * (green - blue) / C;
				}
				if (M == green) {
					H = 120 + 60 * (blue - red) / C;
				}
				if (M == blue) {
					H = 240 + 60 * (red - green) / C;
				}
			}
			else    //grayscale
			{
				H = 0;
			}
			if (H < 0) {
				H = H + 360;
			}

			img1(i, j)[0] = H * 255 / 360;
			img1(i, j)[1] = S * 255;
			img1(i, j)[2] = V * 255;
		}
	}
	return img1;
}

Mat_<Vec3b> HSV_to_RGB(Mat_<Vec3b> img) {
	Mat_<Vec3b> rgb(img.rows, img.cols);

	cvtColor(img, rgb, 54, 0);

	return rgb;
}

Mat_<Vec3b> RGB_to_YCrCb(Mat_<Vec3b> img)
{
	Mat_<Vec3b> ycrcb(img.rows, img.cols);

	cvtColor(img, ycrcb, 36, 0);

	return ycrcb;
}

Mat_<uchar> dilatare(Mat_<uchar> src, Mat_<uchar> str_el)
{
	Mat_<uchar> dst(src.size());
	dst.setTo(255);

	for (int i = 0; i < src.rows;i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src(i, j) == 0)
			{
				for (int u = 0; u < str_el.rows; u++)
				{
					for (int v = 0; v < str_el.cols; v++)
					{
						if (str_el(u, v) == 0)
						{
							int i2 = i + u - str_el.rows / 2;
							int j2 = j + v - str_el.cols / 2;

							if (isInside(dst, i2, j2))
							{
								dst(i2, j2) = 0;
							}
						}
					}
				}
			}
		}
	}
	return dst;
}

Mat_<uchar> evoziune(Mat_<uchar> src, Mat_<uchar> str_el)
{
	Mat_<uchar> dst(src.size());
	dst.setTo(255);
	for (int i = 0; i < src.rows;i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			int ok = 1;
			if (src(i, j) == 0)
			{
				for (int u = 0; u < str_el.rows && ok; u++)
				{
					for (int v = 0; v < str_el.cols && ok; v++)
					{
						if (str_el(u, v) == 0)
						{
							int i2 = i + u - str_el.rows / 2;
							int j2 = j + v - str_el.cols / 2;

							if (!isInside(dst, i2, j2) || src(i2, j2) != 0)
							{
								ok = 0;
							}
						}
					}
				}

				if (ok)
				{
					dst(i, j) = 0;
				}
			}
		}
	}
	return dst;
}

int main()
{
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_FATAL);		//ca sa curatam consola

	Mat_<Vec3b> img_originala = imread("Images/train/35.8.png");		//35.8
	Mat_<Vec3b> img(img_originala.rows, img_originala.cols);
	double scale = 600.0 / img_originala.rows;
	resize(img_originala, img, Size(), scale, scale);			//resize ca sa avem o fereastra mai mica, pastrand ratio-ul

	//bgr in hsv si ycrcb pentru detectia pielii
	Mat_<Vec3b> imagine_HSV = RGB_to_HSV(img);
	Mat_<Vec3b> imagine_ycrcb = RGB_to_YCrCb(img);

	//imshow("imagine", img);
	//imshow("imagine_hsv", imagine_HSV);
	//imshow("imagine_ycrcb", imagine_ycrcb);

	//-----------------------------------////
	//generam mastile pentru cele doua spatii de culoare si apoi le combinam

	Mat_<uchar> mask_hsv;
	Scalar hsv_low(0, 60, 50);
	Scalar hsv_high(20, 255, 255);
	inRange(imagine_HSV, hsv_low, hsv_high, mask_hsv);
	//imshow("mascaHSV", mask_hsv);

	Mat_<uchar> mask_ycrcb;
	Scalar ycrcb_low(0, 135, 85);
	Scalar ycrcb_high(255, 180, 135);
	inRange(imagine_ycrcb, ycrcb_low, ycrcb_high, mask_ycrcb);
	//imshow("mascaYCrCb", mask_ycrcb);

	Mat_<uchar> hsv_and_ycrcb = mask_hsv | mask_ycrcb;
	//imshow("combined mask", hsv_and_ycrcb);
	//
	Mat_<uchar> impuritati = mask_hsv & ~mask_ycrcb;
	Mat_<uchar> mask_v2 = hsv_and_ycrcb & ~impuritati;
	//imshow("Mask V2", hsv_and_ycrcb);

	//dilatare + evoziune--------------------------/////
	Mat_<uchar> elem_structurant(5, 5);
	elem_structurant.setTo(0);
	mask_v2 = dilatare(mask_v2, elem_structurant);
	mask_v2 = evoziune(mask_v2, elem_structurant);

	imshow("dilatare + evoziune", mask_v2);

	mask_v2 = dilatare(mask_v2, elem_structurant);
	mask_v2 = evoziune(mask_v2, elem_structurant);
	imshow("dilatare + evoziune 2", mask_v2);

	//alta metoda pt dilatare+evoziune - cica mai eficienta, dar nu pare
	//Mat_<uchar> mask_v3 = mask_v2.clone();
	//Mat_<uchar> kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	//morphologyEx(mask_v3, mask_v3, MORPH_OPEN, kernel);
	//morphologyEx(mask_v3, mask_v3, MORPH_CLOSE, kernel);
	//imshow("Dupa morfologie V2", mask_v3);



	waitKey();

	return 0;
}