#pragma once
#include <iostream>
#include <vector>
/*
 * 基于 8 邻域像素值的两步迭代算法
 *
 *  P9  P2  P3       I(i-1,j-1) I(i-1,j) I(i-1,j+1)
 *  P8  P1  P4   ->  I(i,j-1)   I(i,j)   I(i,j+1)
 *  P7  P6  P5       I(i+1,j-1) I(i+1,j) I(i+1,j+1)
 *
 *  step1:
 *
 *		循环所有前景像素点，
 *		对符合如下条件的像素点标记为删除:
 *		1.2 <= N(p1) <=6
 *		2.S(P1) = 1 （0-1只跳变了一次）
 *		3.P2 * P4 * P6 = 0
 *		4.P4 * P6 * P8 = 0
 *
 *  step2:
 *
 *		循环所有前景像素点，
 *		对符合如下条件的像素点标记为删除:
 *		1.2 <= N(p1) <=6
 *		2.S(P1) = 1 （0-1只跳变了一次）
 *		3.P2 * P4 * P8 = 0
 *		4.P2 * P6 * P8 = 0
 */

class ZThin
{
private:
	int height_;
	int width_;
	int fc = 1; // 前景像素值
	int bc = 0; // 背景像素值
	char seqence_[8]; // 计算8个像素的跳变
	bool stop1_ = false;
	bool stop2_ = false;
	int* src_ = nullptr;
	int* dst_ = nullptr;
	std::vector<int> pos_res_; // 存储需要改动的像素点
	
	std::vector<std::vector<int>> neibour_ = { {-1,0}, {-1,1},{0, 1},{1, 1},{1, 0},
	                                           {1,-1}, {0,-1},{-1,-1} };
	std::vector<int> values_;
	
	void _thining(int step);

	void _getNeighbour(int i, int j);

	void _isCond(int step, int pos);

	void _update()
	{
		for (auto pos : pos_res_)
		{
			src_[pos] = 0;
		}
	}

	// N(p1)
	bool _cond1()
	{
		int count = 0;
		for (auto value : values_)
			if (value)
				count++;

		return count > 1 && count < 7;
	}

	// S(P1)
	bool _cond2()
	{
		int count = 0;
		for (int i=0;i<7;i++)
		{
			if (seqence_[i] == '0' && seqence_[i + 1] == '1')
				count++;
		}

		return count == 1;
	}

public:
	// 外部分配内存
	explicit ZThin(int* srcData_cpy, int h, int w):height_(h),width_(w)
	{
		src_ = srcData_cpy;
		values_ = std::vector<int>(8, 0);
	}

	void thining()
	{
		// 张-迭代算法
		while (!stop1_ && !stop2_)
		{
			values_.clear();
			pos_res_.clear();
			// step 1
			_thining(0);
			_thining(1);

			_update();
		}
	}
};

inline void ZThin::_getNeighbour(int i, int j)
{
	int idx = 0;
	for (auto ele : neibour_)
	{
		int new_i = i + ele[0];
		int new_j = j + ele[1];
		if (new_i > 0 && new_i < height_ && new_j>0 && new_j < width_)
			values_.push_back(src_[new_i*width_ + new_j]);

		// 存储序列
		seqence_[idx] = static_cast<char>(src_[new_i*width_ + new_j]);
		idx++;
	}
}

inline void ZThin::_isCond(int step, int pos)
{
	if (!_cond1() || !_cond2())
		return;

	int P2 = values_[0];
	int P4 = values_[2];
	int P6 = values_[4];
	int P8 = values_[6];
	
	if (!step)
	{
		if ((P2 * P4 * P6) == 0 && (P4 * P6 * P8) == 0)
		{
			pos_res_.push_back(pos);
			stop1_ = false;
		}
	}
	else
	{
		if ((P2 * P4 * P6) == 0 && (P2 * P6 * P8) == 0)
		{
			pos_res_.push_back(pos);
			stop2_ = false;
		}
	}
}


inline void ZThin::_thining(int step)
{
	if (!step)
		stop1_ = true;
	else
		stop2_ = true;
	
	int pos = 0;
	for (int i=0;i<height_;++i)
	{
		for (int j=0;j<width_;++j)
		{
			pos = i * width_ + j;
			if (!src_[pos])
				continue;
			_getNeighbour(i, j);
			_isCond(step, pos);
		}
	}
}


