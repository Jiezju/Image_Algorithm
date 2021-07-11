#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdio.h>

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CLIP3(x, a, b) MIN2(MAX2(a,x), b)

void GaussMask(int r, double sigma, double gaussmask[])
{
	const double PI = 3.1415926;
	double sum = 0.0;
	int stride = 2 * r + 1;

	// 模板生成
	for (int y = -r, h = 0; y <= r; y++, h++)
	{
		for (int x = -r, w = 0; x<= r; x++, w++)
		{
			gaussmask[w + h*stride] = (1.0 / (2.0 * PI * sigma * sigma)) * (exp(-((double)x * (double)x + (double)y * (double)y) / (2.0 * sigma * sigma)));
			sum += gaussmask[w + h * stride];
		}
	}

	for (int i = 0; i<(stride*stride); i++)
	{
		gaussmask[i] /= sum;
	}
}

void GuassFilter(unsigned char* srcData, int width, int height, int stride, int radius, double sigma)
{
	int offset = stride - 4 * width;
	size_t memsize = sizeof(unsigned char)*height*stride;
	unsigned char* temp = (unsigned char*)malloc(memsize);
	memcpy(temp, srcData, memsize);
	const int eleNum = (2 * radius + 1)*(2 * radius + 1);
	double* gausskernel = (double *)malloc(sizeof(double)*eleNum);
	GaussMask(radius, sigma, gausskernel);
	unsigned char* psrc = srcData;

	int sumr = 0, sumg = 0, sumb = 0;
	for (int j=0; j<height; j++)
	{
		for (int i=0;i<width; i++)
		{
			sumr = sumg = sumb = 0;
			// 遍历每个kernel
			for (int n = -radius; n<= radius; n++)
			{
				for (int m = -radius; m <= radius; m++)
				{
					int ny = CLIP3(j + n, 0, height - 1);
					int nx = CLIP3(i + m, 0, width - 1);
					int pos = nx * 4 + ny * stride;
					sumb += temp[pos] * gausskernel[(m + radius)*radius + (n + radius)];
					sumg += temp[pos + 1] * gausskernel[(m + radius)*radius + (n + radius)];
					sumr += temp[pos + 1] * gausskernel[(m + radius)*radius + (n + radius)];
				}
			}
			psrc[0] = sumb;
			psrc[1] = sumg;
			psrc[2] = sumr;
			psrc += 4;
		}
		psrc += offset;
	}

	free(temp);
	free(gausskernel);
}