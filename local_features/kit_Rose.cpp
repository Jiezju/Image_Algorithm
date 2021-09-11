#include <algorithm>
#include <iostream>
#include <vector>

using namespace  std;

class KitRosePoint
{
private:
	struct Point
	{
		int x_;
		int y_;

		void setPoint(int x, int y)
		{
			this->x_ = x;
			this->y_ = y;
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
	float* Ixx_;
	float* Iyy_;
	float* Ixy_;
	float* dst_;
	float thresh_ = 0.337;

	void _denoise(uint8_t* src, float* dst)
	{
		// Not Implement
		return;
	}
	
	void _init_mem()
	{
		int size = height_ * width_;
		src_ = new float[size];
		Ix_ = new float[size];
		Iy_ = new float[size];
		Ixx_ = new float[size];
		Iyy_ = new float[size];
		Ixy_ = new float[size];
	}

	void _computer_grad(float* src, float* dst, const vector<double> kernel)
	{
		for (int i=0;i<height_; ++i)
		{
			for (int j =0; j<width_;++j)
			{
				int pos = i * width_ + j;
				if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
				{
					dst[pos] = 0;
				}
				else
				{
					// conv with sobel
					float gx = 0.0;
					for (int kx = -1; kx < 2; kx++)
					{
						for (int ky = -1; ky < 2; ky++)
						{
							int kpos = (kx + 1) * 3 + (ky + 1);
							// (i+kx, j+ky)
							int src_pos = (i + kx)*width_ + (j + ky);
							gx += src[src_pos] * kernel[kpos];
						}
					}
				}
				
			}
		}
	}

	void _computer_C()
	{
		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;

				float Ix = Ix_[pos];
				float Iy = Iy_[pos];
				float Ixx = Ixx_[pos];
				float Ixy = Ixy_[pos];
				float Iyy = Iyy_[pos];

				float K = ((1 + Ix * Ix)*Iyy + (1 + Iy * Iy)*Ixx - 2 * Ix*Iy*Ixy) / (2 * (1 + Ix * Ix + Iy * Iy));
				float g = sqrt(Ix * Ix + Iy * Iy);

				dst_[pos] = K * g;
			}
		}
	}

	void _nms()
	{
		for (int i = 0; i < height_; ++i)
		{
			for (int j = 0; j < width_; ++j)
			{
				int pos = i * width_ + j;

				// conv with sobel
				float gmax = 0.0;
				Point pt;
				for (int kx = -1; kx < 2; kx++)
				{
					for (int ky = -1; ky < 2; ky++)
					{
						// (i+kx, j+ky)
						int src_pos = (i + kx)*width_ + (j + ky);
						if (dst_[pos] > thresh_ && dst_[pos]>gmax)
						{
							gmax = dst_[pos];
							pt.setPoint(i + kx, j + ky);
						}
					}
				}
				angles_.push_back(pt);
			}
		}
	}
	
public:
	KitRosePoint(uint8_t* srcdata, int height, int width):height_(height),width_(width)
	{
		// initial
		_init_mem();
		
		// Step 1: guass filter
		_denoise(srcdata, src_);

		// Step2：基于sobel进行梯度计算Ix,Iy,Ixx,Iyy,Ixy
		_computer_grad(Ix_, src_, sobelx_kernel_);
		_computer_grad(Iy_, src_, sobely_kernel_);
		_computer_grad(Ixx_, Ix_, sobelx_kernel_);
		_computer_grad(Iyy_, Iy_, sobely_kernel_);
		_computer_grad(Ixy_, Ix_, sobely_kernel_);

		// Step3：计算各个像素的响应函数C
		_computer_C();

		// Step4：非极大值抑制
		_nms();
	}

	~KitRosePoint()
	{
		delete[] src_;
		delete[] Ix_;
		delete[] Iy_;
		delete[] Ixx_;
		delete[] Iyy_;
		delete[] Ixy_;
	}
	
};
