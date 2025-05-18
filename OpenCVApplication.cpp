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

	Mat_<Vec3b> img_originala = imread("Images/train/35.8.png");
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

	Mat_<uchar> mask_skin = mask_hsv & mask_ycrcb;
	imshow("combined mask", mask_skin);


	//Mat_<Vec3b> mask_color;
	//cvtColor(mask_skin, mask_color, 8, 0);
	//Mat_<Vec3b> skin_only = img & mask_color;

	//imshow("skin only", skin_only);

	//afisam un layer cu pixeli rosii peste imagine, doar acolo unde s-a detectat pielea
	//Mat_<Vec3b> highlight(img.size());
	//for (int i = 0; i < img.rows; i++)
	//{
	//	for (int j = 0; j < img.cols; j++)
	//	{
	//		highlight(i, j)[0] = 0;
	//		highlight(i, j)[1] = 0;
	//		highlight(i, j)[2] = mask_skin(i, j);
	//	}
	//}
	//Mat_<Vec3b> result(img.size());
	//addWeighted(img, 0.4, highlight, 0.6, 0, result);			//suprapunerea imaginilor
	//imshow("highlighted skin", result);



	//dilatare + evoziune--------------------------/////

	//Mat_<uchar> elem_structurant(5, 5);
	//elem_structurant.setTo(0);
	//mask_skin = dilatare(mask_skin, elem_structurant);
	//mask_skin = evoziune(mask_skin, elem_structurant);

	Mat_<uchar> kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	morphologyEx(mask_skin, mask_skin, MORPH_OPEN, kernel);
	morphologyEx(mask_skin, mask_skin, MORPH_CLOSE, kernel);

	imshow("Cleaned Mask", mask_skin);



	waitKey();

	return 0;
}