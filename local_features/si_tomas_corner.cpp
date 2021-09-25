#include <algorithm>
#include <iostream>
#include <vector>

using namespace  std;

class STPoint
{
private:
	struct Point
	{
		int x_;
		int y_;
		float r_;

		void setPoint(int x, int y, float r)
		{
			this->x_ = x;
			this->y_ = y;
			this->r_ = r;
		}

		bool operator>(Point other)
		{
			return this->r_ > other.r_;
		}
		bool operator<(Point other)
		{
			return this->r_ < other.r_;
		}

		bool operator=(Point other)
		{
			return this->r_ = other.r_;
		}
	};

	const vector<double> sobelx_kernel_ = { -1, 0, 1,
												 -2, 0, 2,
												 -1, 0, 1 };
	const vector<double> sobely_kernel_ = { 1, 2, 1,
												 0, 0, 0,
												-1,-2, -1 };

	vector<Point> angles_;
	int height_;
	int width_;
	float* src_;
	float* Ix_;
	float* Iy_;
	float* Ix2_;
	float* Iy2_;
	float* IxIy_;
	float* R_;
	float* dst_;
	float k_;
	vector<Point> pts;
	float thresh_ = 0.337;

	void _denoise(uint8_t* src, float* dst)
	{
		// Not Implement 图像模糊 实现比较简单略
		return;
	}

	void _filter(float* src)
	{
		// Not Implement 图像模糊 实现比较简单略
		return;
	}

	void _init_mem()
	{
		int size = height_ * width_;
		src_ = new float[size];
		Ix_ = new float[size];
		Iy_ = new float[size];
	}

	void _computer_grad(float* src, float* dst, const vector<double> kernel)
	{
		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;
				if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
				{
					dst[pos] = 0;
				}
				else
				{
					// conv with sobel
					float g = 0.0;
					for (int kx = -1; kx < 2; kx++)
					{
						for (int ky = -1; ky < 2; ky++)
						{
							int kpos = (kx + 1) * 3 + (ky + 1);
							// (i+kx, j+ky)
							int src_pos = (i + kx)*width_ + (j + ky);
							g += src[src_pos] * kernel[kpos];
							dst[pos] = g;
						}
					}
				}

			}
		}
	}

	void _compute_C(float* src)
	{
		// 计算 梯度
		_computer_grad(Ix_, src_, sobelx_kernel_);
		_computer_grad(Iy_, src_, sobely_kernel_);

		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;
				Ix2_[pos] = Ix_[pos] * Ix_[pos];
				Iy2_[pos] = Iy_[pos] * Iy_[pos];
				IxIy_[pos] = Ix_[pos] * Iy_[pos];
			}
		}

		_filter(Ix2_);
		_filter(Iy2_);
		_filter(IxIy_);
	}

public:
	STPoint(uint8_t* srcdata, int height, int width, float k) :height_(height), width_(width),k_(k)
	{
		// initial
		_init_mem();

		_denoise(srcdata, src_);

		// step1. 计算自相关矩阵
		_compute_C(src_);

		// 计算 R
		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;
				R_[pos] = Ix2_[pos] * Iy2_[pos] - IxIy_[pos]* IxIy_[pos] - k*(Ix2_[pos] + Iy2_[pos])*(Ix2_[pos] + Iy2_[pos]);
			}
		}

		//  3X3 邻域内 非极大值抑制
		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;
				bool isMax = true;
				// 查找3X3 邻域内的最大值
				for (int k1 = -1; k1 <2; k1++)
				{
					int new_i = i + k1;
					if (new_i<=0 || new_i > height_)
						continue;
					for (int k2 = -1; k2 < 2; k2++)
					{
						int new_j = j + k1;
						if (new_j <= 0 || new_j > height_)
							continue;

						if (R_[pos] < R_[new_i*width_ + new_j])
						{
							R_[pos] = 0.0;
							isMax = false;
						}
					}
					if (!isMax)
						break;
					else
					{
						Point pt;
						pt.setPoint(i, j, R_[pos]);
						pts.push_back(pt);
					}
				}
			}
		}

		// 分块取最大的特征点 , 默认每个 block 9*9
		int blockSize = 9;
		int grid_width = width_ / blockSize;
		int grid_height = height_ / blockSize;

		vector<vector<Point>> grid(grid_width*grid_height);

		for (auto pt:pts)
		{
			int grid_x = pt.x_ / blockSize;
			int grid_y = pt.y_ / blockSize;
			grid[grid_x*grid_width + grid_y].push_back(pt);
		}

		vector<Point> res;
		for (vector<Point> vector_pt: grid)
		{
			// 块内如果有点存在
			if (vector_pt.size()>1)
			{
				sort(vector_pt.begin(), vector_pt.end());
				

				while (!vector_pt.empty())
				{
					Point pivot = vector_pt[vector_pt.size() - 1];
					res.push_back(vector_pt[0]);
					vector_pt.pop_back();

					for (int k =0 ; k< vector_pt.size(); ++k)
					{
						float dx = vector_pt[k].x_ - pivot.x_;
						float dy = vector_pt[k].y_ - pivot.y_;

						if (dx*dx + dy*dy < thresh_)
						{
							vector_pt.erase(vector_pt.begin() + k);
						}
					}
					
				}
				
			}
		}
	}

	~STPoint()
	{
		delete[] src_;
		delete[] Ix_;
		delete[] Iy_;
		delete[] Ix2_;
		delete[] Iy2_;
		delete[] IxIy_;
		delete[] R_;
	}
};