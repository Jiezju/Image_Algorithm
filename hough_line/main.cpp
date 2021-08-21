#include "HoughLine.h"

const char* CW_IMG_ORIGINAL = "Original";
const char* CW_IMG_BINARY = "BinaryImage";
const char* CW_IMG_EDGE = "Hough Line Detection";
const char* img_path = "1.jpg";

int main()
{
	cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(CW_IMG_BINARY, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(CW_IMG_EDGE, cv::WINDOW_AUTOSIZE);

	cv::Mat img = cv::imread(img_path, 0);
	cv::Mat binary;
	cv::threshold(img, binary, 170, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	
	HoughLine hl(binary.cols,binary.rows, 15);
	auto res = hl.detect(binary.data);
	
	cv::imshow(CW_IMG_BINARY, binary);
	
	return 0;
}

