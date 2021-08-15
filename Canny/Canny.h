#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

class Canny
{
private:
	int height_;
	int width_;
	int size_;
	double* gradient_; // �ݶ�ͼ��
	double* degree_; // ���ͼ��
	double* gray_; // �Ҷ�ͼ��
	double highThresh_;
	double lowThresh_;
	double PI_ = 3.1415926;
	double t_ = 180.0 / PI_;
	double gradientMax_ = 0.0;
	
	const std::vector<double> gauss_kernel_ = { 1 / 16.0, 2 / 16.0, 1 / 16.0,
						                        2 / 16.0, 4 / 16.0, 2 / 16.0,  
						                        1 / 16.0, 2 / 16.0, 1 / 16.0 };
	const std::vector<double> sobelx_kernel_ = { -1, 0, 1,
												 -2, 0, 2,
												 -1, 0, 1 };
	const std::vector<double> sobely_kernel_ = { 1, 2, 1,
						                         0, 0, 0,
						                        -1,-2, -1 };
	//  �㷨����
	void _gray_scale(const uint8_t* srcdata);
	void _guassian_filter();
	void _compute_gradient_degree();
	void _non_max_mini();
	void _two_thresh_judge();
	void _edge_tracking();
public:
	explicit Canny(int height, 
		int width, 
		int highThresh, 
		int lowThresh):
		height_(height), width_(width), size_(height*width),
	    lowThresh_(lowThresh), highThresh_(highThresh)
	{
		double* gray_ = new double[size_];
		double* gradient_ = new double[size_];
		double* degree_ = new double[size_];
	}

	~Canny()
	{
		delete[] gray_;
		delete[] gradient_;
		delete[] degree_;
	}

	void canny_edge(const uint8_t* srcdata, uint8_t* dstdata);
};

