int getMaxIndex(const int histom[], const int n)
{
	int max = histom[0];
	int maxIndex = 0;

	for (int i=0; i<n;i++)
	{
		if (histom[i] > max)
		{
			max = histom[i];
			maxIndex = i;
		}
	}
	
	return maxIndex;
}

int getSecondIndex(const int histom[], const int n, int first)
{
	int secondIndex = 0;
	int argmax = 0;
	int argvalue = 0;
	for (int i=0;i<n;i++)
	{
		argvalue = (first - i)*(first - i)*histom[i];
		if (argvalue > argmax)
		{
			secondIndex = i;
			argmax = argvalue;
		}
	}
	
	return secondIndex;
}

int getMinIndex(const int histom[], const int rangL, const int rangR)
{
	int minV = histom[rangL];
	int minIndex = rangL;

	for (int i = rangL + 1; i < rangR; ++i)
	{
		if (minV > histom[i])
		{
			minV = histom[i];
			minIndex = i;
		}
	}

	return minIndex;
	
}


int threshTriangle(unsigned char* src, int width, int height, int stride)
{
	const int binNum = 256;
	int* histom = new int[binNum](); //  initialize 0
	int gray = 0;
	int offset = stride - 4 * width;
	unsigned char* ptr = src;

	for (int i=0;i<height;++i)
	{
		for (int j=0;j<width;++j)
		{
			gray = (ptr[0] + ptr[1] + ptr[2]) / 3;
			histom[gray]++;
		}
		ptr += 4;
	}

	int firstPeak = getMaxIndex(histom, binNum);
	int secondPeak = getSecondIndex(histom, binNum, firstPeak);

	int Thresh = firstPeak < secondPeak ? getMinIndex(histom, firstPeak, secondPeak) : getMinIndex(histom, secondPeak, firstPeak);

	delete[] histom;

	return Thresh;
}

int f_Threshold(unsigned char *srcData, int width, int height, int stride, int T)
{
	int newT = threshTriangle(srcData, width, height, stride);
	T = newT;
	int ret = 0;
	int i, j, gray, offset;
	offset = stride - width * 4;
	unsigned char* pSrc = srcData;
	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			gray = (pSrc[0] + pSrc[1] + pSrc[2]) / 3;
			gray = gray < T ? 0 : 255;
			pSrc[0] = gray;
			pSrc[1] = gray;
			pSrc[2] = gray;
			pSrc += 4;
		}
		pSrc += offset;
	}
	return ret;
}

