#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <map>

struct hline
{
	int p;
	int theta;
};

class HoughLine
{
private:
	int num_p_;
	int width_;
	int height_;
	int num_theta_;
	int** vote_board_=nullptr; // 二维投票器
	int thresh_;

public:
	// 构造函数 初始化p_，width_，height_，theta_，vote_board_，thresh_
	HoughLine(int w, int h, int thresh);
	std::vector<hline> detect(const uint8_t* srcdata);
	~HoughLine()
	{
		for (int p=0; p<num_p_; ++p)
		{
			free(vote_board_[p]);
		}
	}
};

