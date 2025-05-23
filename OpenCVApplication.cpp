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

Mat_<uchar> eroziune(Mat_<uchar> src, Mat_<uchar> str_el)
{
	Mat_<uchar> dst(src.size());
	dst.setTo(255);
	for (int i = 0;i < src.rows;i++)
		for (int j = 0;j < src.cols;j++)
			if (src(i, j) == 0)
			{
				for (int u = 0;u < str_el.rows;u++)
					for (int v = 0;v < str_el.cols;v++)
						if (str_el(u, v) == 0)
						{
							int i2 = i + u - str_el.rows / 2;
							int j2 = j + v - str_el.cols / 2;
							if (isInside(dst, i2, j2))
							{
								if (src(i2, j2) == 255)
									goto afara;

							}
						}

				dst(i, j) = 0;
			afara:
				if (0);
			}
	return dst;
}

Mat_<uchar> dilatare(Mat_<uchar> src, Mat_<uchar> str_el) {
	Mat_<uchar> dst(src.size());
	dst.setTo(255);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			if (src(i, j) == 0) {
				for (int u = 0; u < str_el.rows; u++) {
					for (int v = 0; v < str_el.cols; v++) {
						if (str_el(u, v) == 0) {
							int i2 = i + u - str_el.rows / 2;
							int j2 = j + v - str_el.cols / 2;
							if (isInside(dst, i2, j2)) {
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

Mat_<int> etichetare_v2(Mat_<uchar> img)
{
	int label = 0;
	Mat_<int> labels(img.size());
	labels.setTo(0);
	vector<vector<int>> edges(1000);

	int di[4] = { 0, -1, -1, -1 };
	int dj[4] = { -1, -1, 0, 1 };

	for (int i = 0; i < labels.rows;i++)
	{
		for (int j = 0; j < labels.cols; j++)
		{
			if (img(i, j) == 255 && labels(i, j) == 0)
			{
				vector<int> L;
				for (int k = 0; k < 4; k++)
				{
					int i2 = i + di[k];
					int j2 = j + dj[k];

					if (isInside(labels, i2, j2) && labels(i2, j2) > 0)
					{
						L.push_back(labels(i2, j2));
					}
				}
				if (L.size() == 0)
				{
					label++;
					labels(i, j) = label;
				}
				else
				{
					int x = *min_element(L.begin(), L.end());
					labels(i, j) = x;
					for (int y : L)
					{
						if (y != x)
						{
							edges[x].push_back(y);
							edges[y].push_back(x);
						}
					}
				}
			}
		}
	}

	int new_label = 0;
	vector<int> new_labels(label + 1);

	for (int i = 0; i < label + 1; i++)
	{
		new_labels[i] = 0;
	}

	for (int i = 1; i <= label; i++)
	{
		if (new_labels[i] == 0)
		{
			new_label++;
			queue<int> Q;
			new_labels[i] = new_label;
			Q.push(i);
			while (Q.size())
			{
				int x = Q.front();
				Q.pop();

				for (int y : edges[x])
				{
					if (new_labels[y] == 0)
					{
						new_labels[y] = new_label;
						Q.push(y);
					}
				}
			}
		}
	}

	for (int i = 0; i < labels.rows;i++)
	{
		for (int j = 0; j < labels.cols; j++)
		{
			labels(i, j) = new_labels[labels(i, j)];
		}
	}
	return labels;
}

Mat_<Vec3b> colorare(Mat_<int> img)
{
	Mat_<Vec3b> img2(img.size());
	int maxim = 0;
	for (int i = 0; i < img.rows;i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			maxim = max(maxim, img(i, j));
		}
	}

	vector<int> labels_count(maxim + 1, 0);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			labels_count[img(i, j)]++;
		}
	}

	int first_max = -1, second_max = -1;
	int first_idx = -1, second_idx = -1;

	for (int i = 0; i <= maxim; i++)
	{
		if (labels_count[i] > first_max)
		{
			second_max = first_max;
			second_idx = first_idx;
			first_max = labels_count[i];
			first_idx = i;
		}
		else if (labels_count[i] > second_max)
		{
			second_max = labels_count[i];
			second_idx = i;
		}
	}

	int skin_label = second_idx;

	cout << "background: " << first_idx << ", piele: " << skin_label << endl;

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			if (img(i, j) == skin_label)
			{
				img2(i, j) = Vec3b(0, 0, 0);			//skin
			}
			else {
				img2(i, j) = Vec3b(255, 255, 255);		//restul
			}
		}
	}

	return img2;
}

Mat_<uchar> binarizare(Mat_<uchar> img, int prag)
{
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) < prag) {
				img(i, j) = 0;
			}
			else {
				img(i, j) = 255;
			}
		}
	}
	return img;
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

	////-----------------------------------////
	////generam mastile pentru cele doua spatii de culoare si apoi le combinam

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

	Mat_<uchar> mask_combined = mask_hsv | mask_ycrcb;
	//imshow("combined mask", mask_combined);

	////dilatare + eroziune--------------------------/////
	Mat_<uchar> elem_structurant(10,10);
	elem_structurant.setTo(0);
	mask_combined = dilatare(mask_combined, elem_structurant);
	mask_combined = eroziune(mask_combined, elem_structurant);

	imshow("imagine dupa dilatare si eroziune", mask_combined);

	////etichetare + colorare-----------------------////
	Mat_<int> img_etichetata = etichetare_v2(mask_combined);
	Mat_<Vec3b> img_et_col = colorare(img_etichetata);

	imshow("imagine dupa etichetare si colorare", img_et_col);


	//--------------------------------------------///////
	//Canny


	//--------------------------------------------///////
	//crop + afisare abdomen


	//--------------------------------------------///////
	//calculare bodyfat

	waitKey();

	return 0;
}