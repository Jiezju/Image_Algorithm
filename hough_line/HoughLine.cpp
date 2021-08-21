#include "HoughLine.h"

HoughLine::HoughLine(int w, int h, int thresh) : width_(w), height_(h), thresh_(thresh)
{
	// step 1. 初始化 投票器
	num_p_ = 2 * sqrt(float(width_*width_) + float(height_*height_)) + 1;
	num_theta_ = 181;

	vote_board_ = static_cast<int**>(malloc(num_p_ * sizeof(int)));
	if (!vote_board_)
	{
		printf("mem err\n");
		return;
	}

	for (int i=0;i<num_p_;++i)
	{
		vote_board_[i] = static_cast<int*>(malloc(num_theta_ * sizeof(int)));
		if (vote_board_[i] == NULL)
		{
			printf("buf err\n");
			return;
		}
		memset(vote_board_[i], 0, num_theta_ * sizeof(int));
	}
}

std::vector<hline> HoughLine::detect(const uint8_t* srcdata)
{
	std::vector<hline> lines_;
	// step 2.遍历图像 标记投票器 vote_board_[p][theta]
	int p = 0;
	int offset = num_p_ / 2; // 对负数的theta进行偏移
	for (int i=0;i<height_;++i)
	{
		for (int j=0;j<width_;++j)
		{
			// 只检测 像素值 255 的有效像素点
			int pos = i * width_ + j;
			uint8_t value = srcdata[pos];
			if (value == 0)
				continue;

			for (int theta = 0; theta <181; ++theta)
			{
				p = j * sin(theta / 180 * CV_PI) + i * cos(theta / 180 * CV_PI) + offset;
				if (p<0)
				{
					std::cout << "warrning" << std::endl;
					continue;
				}
				vote_board_[p][theta]++;
			}
		}
	}

	// step 3.选择超过阈值的直线
	for (int p=0;p<num_p_;++p)
	{
		for (int theta = 0; theta < num_theta_; ++theta)
		{
			if (vote_board_[p][theta] <= thresh_)
				continue;

			hline line;
			line.p = p - offset;
			line.theta = theta;
			lines_.push_back(line);
		}
	}

	// step 4.进一步删除接近的直线（优化）
	/*
	 * 思路：依据p,theta的相差范围，选择删除误差一定区间内的直线
	 */
	std::map<std::pair<int, int>, int> line_map;
	while (!lines_.empty())
	{
		hline l = lines_.back();
		lines_.pop_back();
		int max_theta = l.theta + 4;
		int min_theta = l.theta - 4;
		int max_p = l.p + 2;
		int min_p = l.p - 2;

		bool flag = false;
		for (int sp = min_p; sp < max_p; sp++)
		{
			for(int stheta = min_theta;stheta < max_theta; stheta++)
			{
				if (line_map.count(std::pair<int, int>(sp, stheta)))
				{
					flag = true;
				}
			}
		}

		if (!flag)
		{
			line_map[std::pair<int, int>(l.p, l.theta)] = 1;
		}
	}

	auto iter = line_map.begin();

	while (iter != line_map.end())
	{
		hline l;
		l.p = iter->first.first;
		l.theta = iter->first.second;
		lines_.push_back(l);
	}

	return lines_;
}
