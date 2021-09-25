#include<iostream>
#include<stdlib.h>
#include<math.h>

inline float getMax(const float &b, const float &g, const float &r)
{
	double Max = b > g ? b : g;
	Max = b > Max ? b : Max;
	return Max;
}

inline float getMin(const float &b, const float &g, const float &r)
{
	double Min = b > g ? g : b;
	Min = b > Min ? Min : b;
	return Min;
}

float computeH(const float r, const float g, const float b, const float Max, const float Min)
{
	double s = 60 / (Max - Min);
	if (Max == Min)
	{
		return 0.0;
	}
	else if (Max == r && g>=b)
	{
		return (g - b)*s;
	}
	else if (Max == r && g < b)
	{
		return (g - b)*s + 360;
 	}
	else if (Max == g)
	{
		return (b - r)*s + 120;
	}
	else if (Max == b)
	{
		return (r - g)*s + 240;
	}
	else
	{
		return 125.0;
	}
}

float computeS(const float Max, const float Min)
{
	return Max == 0 ? 0 : ((Max - Min) / Max);
}

void RGB2HSV(const unsigned char &R, const unsigned char &G, const unsigned char &B, float* h, float* s, float * v)
{
	float r = R * scale;
	float g = G * scale;
	float b = B * scale;
	float Max = getMax(b, g, r);
	float Min = getMin(b, g, r);
	*h = computeH(r, g, b, Max, Min);
	*s = computeS(Max, Min);
	*v = Max;
}

float clip(float value)
{
	if (value > 255)
	{
		return 255.0;
	}
	else if (value < 0.0)
	{
		return 0.0;
	}
	else
	{
		return value;
	}
}


void HSV2RGB(const float &h, const float &s, const float &v, unsigned char* R, unsigned char *G, unsigned char *B)
{
	float h_i = h / 60;
	float f = h / 60 - h;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f)*s);
	float r, g, b;

	switch (int(h_i))
	{
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = p;
		g = q;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	default:
		break;
	}
	*R = static_cast<unsigned char>(clip(r * 255));
	*G = static_cast<unsigned char>(clip(g * 255));
	*B = static_cast<unsigned char>(clip(b * 255));
}

int f_HSVAdjust(unsigned char *srcData, int width, int height, int stride, float hIntensity, float sIntensity, float vIntensity)
{
	int ret = 0;
	if(srcData == NULL)
	{
		printf("input image is null!");
		return -1;
	}
	//Process
    unsigned char R, G, B;
	float h = 0, s = 0, v = 0;
	unsigned char* pSrc = srcData;
	int offset = stride - width * 4; // 对齐
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			B = pSrc[0];
			G = pSrc[1];
			R = pSrc[2];
			RGB2HSV(R, G, B, &h, &s, &v);
			h = h + hIntensity > 360 ? h + hIntensity - 360 : h + hIntensity;
			s = CLIP3(s + sIntensity, 0, 1.0f);
			v = CLIP3(v + vIntensity, 0, 1.0f);
			HSV2RGB(h, s, v, &R, &G, &B);
			pSrc[0] = B;
			pSrc[1] = G;
			pSrc[2] = R;
			pSrc += 3;
		}
		pSrc += offset;
	}
	return ret;
}

