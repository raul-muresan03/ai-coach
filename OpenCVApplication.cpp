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

Mat_<uchar> color_to_grayscale(Mat_<Vec3b> img) {
	Mat_<uchar> img1(img.rows, img.cols);

	for (int i = 0; i < img.rows;i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			img1(i, j) = (img(i, j)[0] + img(i, j)[1] + img(i, j)[2]) / 3;
		}
	}

	return img1;
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

int isInside(Mat img, int i, int j) {
	return i >= 0 && i < img.rows && j >= 0 && j < img.cols;
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


	waitKey();

	return 0;
}