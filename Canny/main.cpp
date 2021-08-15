#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "canny.h"


std::string img_path;
int low_threshold = 30;
int high_threshold = 100;

const char* CW_IMG_ORIGINAL = "Original";
const char* CW_IMG_GRAY = "Grayscale";
const char* CW_IMG_EDGE = "Canny Edge Detection";

void doTransform(std::string);


int main()
{

	const char* img_path = "1.jpg";


    cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_IMG_GRAY, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_IMG_EDGE, cv::WINDOW_AUTOSIZE);


    doTransform(img_path);

	return 0;
}

void doTransform(std::string file_path)
{
    cv::Mat img_gray;

    cv::Mat img_ori = cv::imread(file_path, 1);
	if (img_ori.empty())
	{
		std::cout << "No image" << std::endl;;
	}
    cv::cvtColor(img_ori, img_gray, cv::COLOR_BGR2GRAY);

    int w = img_gray.cols;
    int h = img_ori.rows;

    cv::Mat img_edge(h, w, CV_8UC1, cv::Scalar::all(0));

    Canny canny(w, h, high_threshold,low_threshold);
    canny.canny_edge_detect(img_gray.data, img_edge.data);

    // Visualize all
    cv::imshow(CW_IMG_ORIGINAL, img_ori);
    cv::imshow(CW_IMG_GRAY, img_gray);
    cv::imshow(CW_IMG_EDGE, img_edge);

}
