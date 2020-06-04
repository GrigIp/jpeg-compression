// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#define pi 3.142857

typedef struct RLE_Component {
	char value;
	char occurence;
};

Mat dctTransformAndQuantize(Mat src) {

	char quantizationData[8][8] = {
		{ 16, 11, 10, 16, 24, 40, 51, 61 },
		{ 12, 12, 14, 19, 26, 58, 60, 55 },
		{ 14, 13, 16, 24, 40, 57, 69, 56 },
		{ 14, 17, 22, 29, 51, 87, 80, 62 },
		{ 18, 22, 37, 56, 68, 109, 103, 77 },
		{ 24, 35, 55, 64, 81, 104, 113, 92 },
		{ 49, 64, 78, 87, 103, 121, 120, 101 },
		{ 72, 92, 95, 98, 112, 100, 103, 99 }
	};

	Mat quantizationMat = Mat(8, 8, CV_8S, quantizationData);
	Mat dst = Mat::zeros(8, 8, CV_8S);
	
	int height = dst.rows;
	int width = dst.cols;

	float ci, cj, dct, sum;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			if (i == 0) {
				ci = 1 / sqrt(8);
			}
			else {
				ci = sqrt(2) / sqrt(8);
			}

			if (j == 0) {
				cj = 1 / sqrt(8);
			}
			else {
				cj = sqrt(2) / sqrt(8);
			}

			sum = 0;
			for (int k = 0; k < height; k++) {
				for (int l = 0; l < width; l++) {
					dct = (float)src.at<char>(k, l) *
						cos((2 * k + 1) * i * pi / (2 * height)) *
						cos((2 * l + 1) * j * pi / (2 * width));
					sum = sum + dct;
				}
			}
			dst.at<char>(i, j) = (char)round((ci * cj * sum) / (float)quantizationMat.at<char>(i, j));
		}
	}
	return dst;
}

void zigZagTraversal(Mat src, char zigZag[])
{
	int n = src.rows;
	int m = src.cols;
	int row = 0, col = 0;
	int zigZagCnt = 0;

	bool row_inc = 0;
 
	int mn = min(m, n);
	for (int len = 1; len <= mn; ++len) {
		for (int i = 0; i < len; ++i) {
			zigZag[zigZagCnt++] = (int)src.at<char>(row,col);

			if (i + 1 == len)
				break;

			if (row_inc)
				++row, --col;
			else
				--row, ++col;
		}

		if (len == mn)
			break;
 
		if (row_inc)
			++row, row_inc = false;
		else
			++col, row_inc = true;
	}
 
	if (row == 0) {
		if (col == m - 1)
			++row;
		else
			++col;
		row_inc = 1;
	}
	else {
		if (row == n - 1)
			++col;
		else
			++row;
		row_inc = 0;
	}
 
	int MAX = max(m, n) - 1;
	for (int len, diag = MAX; diag > 0; --diag) {

		if (diag > mn)
			len = mn;
		else
			len = diag;

		for (int i = 0; i < len; ++i) {
			zigZag[zigZagCnt++] = (int)src.at<char>(row, col);

			if (i + 1 == len)
				break;

			if (row_inc)
				++row, --col;
			else
				++col, --row;
		}

		if (row == 0 || col == m - 1) {
			if (col == m - 1)
				++row;
			else
				++col;

			row_inc = true;
		}

		else if (col == 0 || row == n - 1) {
			if (row == n - 1)
				++col;
			else
				++row;

			row_inc = false;
		}
	}
}

Mat zigZagReverseTraversal(char zigZag[])
{
	Mat dst = Mat::zeros(8, 8, CV_8S);
	int n = dst.rows;
	int m = dst.cols;
	int row = 0, col = 0;
	int zigZagCnt = 0;

	bool row_inc = 0;

	int mn = min(m, n);
	for (int len = 1; len <= mn; ++len) {
		for (int i = 0; i < len; ++i) {
			dst.at<char>(row, col) = zigZag[zigZagCnt++];

			if (i + 1 == len)
				break;

			if (row_inc)
				++row, --col;
			else
				--row, ++col;
		}

		if (len == mn)
			break;

		if (row_inc)
			++row, row_inc = false;
		else
			++col, row_inc = true;
	}

	if (row == 0) {
		if (col == m - 1)
			++row;
		else
			++col;
		row_inc = 1;
	}
	else {
		if (row == n - 1)
			++col;
		else
			++row;
		row_inc = 0;
	}

	int MAX = max(m, n) - 1;
	for (int len, diag = MAX; diag > 0; --diag) {

		if (diag > mn)
			len = mn;
		else
			len = diag;

		for (int i = 0; i < len; ++i) {
			dst.at<char>(row, col) = zigZag[zigZagCnt++];

			if (i + 1 == len)
				break;

			if (row_inc)
				++row, --col;
			else
				++col, --row;
		}

		if (row == 0 || col == m - 1) {
			if (col == m - 1)
				++row;
			else
				++col;

			row_inc = true;
		}

		else if (col == 0 || row == n - 1) {
			if (row == n - 1)
				++col;
			else
				++row;

			row_inc = false;
		}
	}
	return dst;
}

std::vector<RLE_Component> runLengthEncode(char arr[]) {
	std::vector<RLE_Component> rle_components = std::vector<RLE_Component>();

	for (int i = 0; i < 64; i++) {
		int count = 1;
		while (i < 64 && arr[i] == arr[i + 1]) {
			count++;
			i++;
		}

		RLE_Component rle_component;
		rle_component.value = arr[i];
		rle_component.occurence = count;

		rle_components.push_back(rle_component);
	}

	rle_components.pop_back();

	return rle_components;
}


std::vector<std::vector<RLE_Component>> compress(Mat src)
{		
	
	//Mat srcYCrCb;
	//cv::cvtColor(src, srcYCrCb, cv::COLOR_BGR2YCrCb);

	int height = src.rows;
	int width = src.cols;
	Mat imageBlock = Mat::zeros(8, 8, CV_8S);
	std::vector<std::vector<RLE_Component>> rleComponents = std::vector<std::vector<RLE_Component>>();;
	for (int i = 0; i < height; i += 8) {
		for (int j = 0; j < width; j += 8) {
			int ii = i;
			int iii = 0;
			while (ii < i + 8) {
				int jj = j;
				int jjj = 0;

				while (jj < j + 8) {
					if (ii < height && jj < width) {
						imageBlock.at<char>(iii, jjj) = (char)(src.at<uchar>(ii, jj) - 128);
						//imageBlock.at<char>(iii, jjj) = (char)(srcYCrCb.at<uchar>(ii, jj) - 128);
					}
					else {
						imageBlock.at<char>(iii, jjj) = 0;
					}
					jjj++;
					jj++;
				}
				iii++;
				ii++;
			}
				
			Mat quantiziedMat = dctTransformAndQuantize(imageBlock);
			char zigZagArray[64];

			zigZagTraversal(quantiziedMat, zigZagArray);
			std::vector<RLE_Component> rleComponent = runLengthEncode(zigZagArray);
			rleComponents.push_back(rleComponent);
		}
	}

	return rleComponents;
}

void decodeRle(std::vector<RLE_Component> rleVector, char arr[]) {
	int i = 0;
	for (std::vector<RLE_Component>::iterator rleComponent = rleVector.begin(); rleComponent != rleVector.end(); ++rleComponent) {
		int j = rleComponent->occurence;
		while (j != 0) {
			arr[i++] = rleComponent->value;
			j--;
		}
	}

}

Mat dequantize(Mat src) {
	char quantizationData[8][8] = {
		{ 16, 11, 10, 16, 24, 40, 51, 61 },
		{ 12, 12, 14, 19, 26, 58, 60, 55 },
		{ 14, 13, 16, 24, 40, 57, 69, 56 },
		{ 14, 17, 22, 29, 51, 87, 80, 62 },
		{ 18, 22, 37, 56, 68, 109, 103, 77 },
		{ 24, 35, 55, 64, 81, 104, 113, 92 },
		{ 49, 64, 78, 87, 103, 121, 120, 101 },
		{ 72, 92, 95, 98, 112, 100, 103, 99 }
	};
	Mat quantizationMat = Mat(8, 8, CV_8S, quantizationData);
	Mat dst = Mat::zeros(8, 8, CV_32F);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			dst.at<float>(i, j) = (float)src.at<char>(i, j)*(float)quantizationMat.at<char>(i, j);
		}
	}

	return dst;
}

Mat inverseDct(Mat src) {
	//Mat dst = Mat::zeros(8, 8, CV_32FC1);
	Mat dst = Mat::zeros(8, 8, CV_8U);

	int height = src.rows;
	int width = src.cols;

	float ci, cj, sum;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			sum = 0;
			for (int ii = 0; ii < height; ii++) {
				for (int jj = 0; jj < width; jj++) {

					if (i == 0) {
						ci = 1 / sqrt(2);
					}
					else {
						ci = 1;
					}

					if (j == 0) {
						cj = 1 / sqrt(2);
					}
					else {
						cj = 1;
					}
					sum += ci*cj*src.at<float>(ii, jj)*
						cos((float)((2 * i + 1)*ii*pi) / (float)(2 * height))*
						cos((float)((2 * j + 1)*jj*pi) / (float)(2 * width));
				}
			}
			dst.at<uchar>(i, j) = (uchar)((char)roundf((2*sum)/sqrt(height*width)) + 128);
		}
	}

	return dst;
}

Mat decompress(std::vector<std::vector<RLE_Component>> rleComponents, Size size) {

	Mat image = Mat::zeros(size, CV_8U);
	int height = image.rows;
	int width = image.cols;
	for (int i = 0; i < height; i+=8) {
		for(int j = 0; j < width; j+=8)
		for (std::vector<std::vector<RLE_Component>>::iterator itComponents = rleComponents.begin(); itComponents != rleComponents.end(); ++itComponents) {
			std::vector<RLE_Component> itComponent = *itComponents;
			char zigZagArr[64] = { 0 };
			decodeRle(itComponent, zigZagArr);

			Mat quantizedMat = zigZagReverseTraversal(zigZagArr);
			Mat dctMat = dequantize(quantizedMat);
			Mat imageBlock = inverseDct(dctMat);

			int ii = i;
			int iii = 0;
			while (ii < i + 8) {
				int jj = j;
				int jjj = 0;
				while (jj < j + 8) {
					if (ii < height && jj < width) {
						image.at<uchar>(ii, jj) = imageBlock.at<uchar>(iii, jjj);
					}
					jjj++;
					jj++;
				}
				iii++;
				ii++;
			}
			std::cout << i << "--" << j << "\n";
		}
	}
	//Mat imageRGB;
	//cv::cvtColor(image, imageRGB, cv::COLOR_YCrCb2BGR);
	
	//return imageRGB;
	return image;
}



int main()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		Size size = src.size();
		std::vector<std::vector<RLE_Component>> rleComponents = compress(src);

		/*for (std::vector<std::vector<RLE_Component>>::iterator itComponents = rleComponents.begin(); itComponents != rleComponents.end(); ++itComponents) {
			for (std::vector<RLE_Component>::iterator itComponent = itComponents->begin(); itComponent != itComponents->end(); ++itComponent) {
				if (itComponent->occurence == 1) {
					std::cout << (int)itComponent->value << " ";
				}
				else {
					std::cout << '{' << (int)itComponent->occurence << 'x' << (int)itComponent->value << '}' << " ";
				}
			}
			std::cout << std::endl << std::endl << std::endl;
		}*/


		Mat dst = decompress(rleComponents, size);
		imshow("original", src);
		imshow("After Compression and Decompression", dst);
		waitKey();
	}

	return 0;
}