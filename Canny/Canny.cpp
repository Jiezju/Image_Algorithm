#include "Canny.h"


void Canny::canny_edge(const uint8_t* srcdata, uint8_t* dstdata)
{
	// step1: gray scale
	_gray_scale(srcdata);

	// step2: 高斯模糊
	_guassian_filter();

	// step3: 计算梯度幅值和方向
	_compute_gradient_degree();

	// step4: 非最大值抑制
	_non_max_mini();

	// step5: 双阀值
	_two_thresh_judge();

	// step6: 滞后边界跟踪
	_edge_tracking();
}

void Canny::_gray_scale(const uint8_t* srcdata)
{
	double* pGray = gray_;
	const uint8_t* pSrc = srcdata;
	for (int i = 0; i < height_; i++)
	{
		for (int j = 0; j < width_; j++)
		{
			double value = (299 * pSrc[2] + 587 * pSrc[1] + 114 * pSrc[0]) / 1000.0;
			*pGray = value;
			pGray++;
			pSrc += 3;
		}
	}
}

void Canny::_guassian_filter()
{
	for (int i = 0; i < height_; i++)
	{
		for (int j = 0;j < width_; j++)
		{
			int pos = i * width_ + j;

			if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
			{
				gray_[pos] = 0;
			}
			else
			{   // conv with guassian
				double value = 0.0;
				for (int kx = -1; kx < 2; kx++)
				{
					for (int ky = -1; ky < 2; ky++)
					{
						int kpos = (kx + 1) * 3 + (ky + 1);
						value += gray_[pos + (kx * 3) + ky] * gauss_kernel_[kpos];
					}
				}
				gray_[pos] = value;
			}
		}
	}
}

void Canny::_compute_gradient_degree()
{
	for (int i = 0; i < height_; i++)
	{
		for (int j = 0;j < width_; j++)
		{
			int pos = i * width_ + j;

			if (i == 0 || j == 0 || i == height_ - 1 || j == width_ - 1)
			{
				gradient_[pos] = 0;
				degree_[pos] = 0;
			}
			else
			{   // conv with sobel
				double gx = 0.0;
				for (int kx = -1; kx < 2; kx++)
				{
					for (int ky = -1; ky < 2; ky++)
					{
						int kpos = (kx + 1) * 3 + (ky + 1);
						gx += gray_[pos + (kx * 3) + ky] * sobelx_kernel_[kpos];
					}
				}

				double gy = 0.0;
				for (int kx = -1; kx < 2; kx++)
				{
					for (int ky = -1; ky < 2; ky++)
					{
						int kpos = (kx + 1) * 3 + (ky + 1);
						gy += gray_[pos + (kx * 3) + ky] * sobely_kernel_[kpos];
					}
				}

				// gradient compute
				gradient_[pos] = sqrt(gx * gx + gy * gy);
				gradientMax_ = std::max(gradient_[pos], gradientMax_);

				// angle compute
				double angle;
				if (gx == 0)
				{
					angle = (gy == 0) ? 0 : 90;
				}
				else
				{
					double div = gy / gx;
					if (div < 0)
					{
						angle = (int)(180 - atan(-div) * t_);
					}
					else
					{
						angle = (int)(atan(div) * t_);
					}
					if (angle < 22.5f)
					{
						angle = 0;
					}
					else if (angle < 67.5f)
					{
						angle = 45;
					}
					else if (angle < 112.5f)
					{
						angle = 90;
					}
					else if (angle < 157.5f)
					{
						angle = 135;
					}
					else
						angle = 0;
				}
				degree_[pos] = angle;
			}
		}
	}
}

void Canny::_non_max_mini()
{
	// 比较当前点的梯度强度和正负梯度方向点的梯度强度
	// 如果当前点的梯度强度和同方向的其他点的梯度强度相比较是最大，保留其值。否则抑制，即设为0。
	float leftPixel = 0, rightPixel = 0;
	int pos;
	for (int i = 1; i < height_ - 1; i++)
	{
		for (int j = 1; j < width_ - 1; j++)
		{
			pos = j + i * width_;
			switch (int(degree_[pos]))
			{
			case 0:
				leftPixel = gradient_[pos - 1];
				rightPixel = gradient_[pos + 1];
				break;
			case 45:
				leftPixel = gradient_[pos - 1 + width_];
				rightPixel = gradient_[pos + 1 - width_];
				break;
			case 90:
				leftPixel = gradient_[pos + width_];
				rightPixel = gradient_[pos - width_];
				break;
			case 135:
				leftPixel = gradient_[pos + 1 + width_];
				rightPixel = gradient_[pos - 1 - width_];
				break;
			default:
				break;
			}
			if ((gradient_[pos] < leftPixel) || (gradient_[pos] < rightPixel))
			{
				gray_[pos] = 0;
			}
			else
			{
				gray_[pos] = (int)(255.0f * gradient_[pos] / gradientMax_);
			}
		}
	}
}

void Canny::_two_thresh_judge()
{
	// 一个高阀值和一个低阀值来区分边缘像素。
	// 如果边缘像素点梯度值大于高阀值，则被认为是强边缘点。
	// 如果边缘梯度值小于高阀值，大于低阀值，则标记为弱边缘点。
	// 小于低阀值的点则被抑制掉。
	int pos = 0;
	for (int i = 1; i < height_ - 1; i++)
	{
		for (int j = 1; j < width_ - 1; j++)
		{
			pos = j + i * width_;
			if (gray_[pos] > highThresh_)
			{
				gray_[pos] = 255;
			}
			else if (gray_[pos] < lowThresh_)
			{
				gray_[pos] = 0;
			}
			else
			{
				if (gray_[pos - 1 - width_] < highThresh_ && gray_[pos - width_] < highThresh_ && gray_[pos + 1 - width_] < highThresh_ && gray_[pos - 1] < highThresh_
					&& gray_[pos + 1] < highThresh_ && gray_[pos - 1 + width_] < highThresh_ && gray_[pos + width_] < highThresh_ && gray_[pos + 1 + width_] < highThresh_)
				{
					gray_[pos] = 0;
				}
				else
					gray_[pos] = 255;
			}
		}
	}
}


void Canny::_edge_tracking()
{
	// 滞后边界跟踪算法检查一个弱边缘点的8连通领域像素，只要有强边缘点存在，那么这个弱边缘点被认为是真是边缘保留下来。
	int pos = 0;
	for (int i = 1; i < height_ - 1; i++)
	{
		for (int j = 1; j < width_ - 1; j++)
		{
			pos = j + i * width_;
			if (gray_[pos] == 255) {
				gray_[pos] = 255;
			}
			else if (gray_[pos] == 100) {
				if (gray_[pos - 1] == 255 || gray_[pos + 1] == 255 ||
					gray_[pos - 1 - width_] == 255 ||
					gray_[pos + 1 - width_] == 255 || gray_[pos + width_] == 255 ||
					gray_[pos + width_ - 1] == 255 ||
					gray_[pos + width_ + 1] == 255) {
					gray_[pos] = 255;
				}
				else {
					gray_[pos] = 0;
				}
			}
			else {
				gray_[pos] = 0;
			}
		}
	}
}


