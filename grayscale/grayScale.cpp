static inline unsigned char Max(unsigned char a, unsigned char b)
{
	return a > b ? a : b;
}

static inline unsigned char Min(unsigned char a, unsigned char b)
{
	return a < b ? a : b;
}


int f_Gray(unsigned char *srcData, int width, int height, int stride, int mode)
{
	int ret = 0;
	int i, j, gray, offset;
	offset = stride - width * 4;
	unsigned char* pSrc = srcData;
	switch(mode) {
	case 0://mean gray method
		for(j = 0; j < height; j++) {
			for(i = 0; i < width; i++) {
				gray = (pSrc[0] + pSrc[1] + pSrc[2]) / 3;
				pSrc[0] = gray;
				pSrc[1] = gray;
				pSrc[2] = gray;
				pSrc += 3;
			}
			pSrc += offset;
		}
		break;
	case 1://classic gray method
		for(j = 0; j < height; j++) {
			for(i = 0; i < width; i++) {
				gray = (299 *  pSrc[2] + 587 * pSrc[1] + 114 * pSrc[0]) / 1000;
				pSrc[0] = gray;
				pSrc[1] = gray;
				pSrc[2] = gray;
				pSrc += 3;
			}
			pSrc += offset;
		}
		break;
	case 2://photoshop gray method
		for(j = 0; j < height; j++) {
			for(i = 0; i < width; i++) {
				gray = (Max(pSrc[0], Max(pSrc[1], pSrc[2])) + MIN2(pSrc[0], MIN2(pSrc[1], pSrc[2]))) / 2;
				pSrc[0] = gray;
				pSrc[1] = gray;
				pSrc[2] = gray;
				pSrc += 3;
			}
			pSrc += offset;
		}
		break;
	default:
		break;
	}
	return ret;
};

