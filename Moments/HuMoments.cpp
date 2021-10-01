#include "HuMoments.h"


HuMoments::HuMoments(unsigned char* binaryImage, int width, int height, bool islog): width_(width), height_(height),object_(binaryImage), islog_(islog)
{
	calcInvariantMoments();
	if (islog_)
		logTransform();
}

HuMoments::~HuMoments() = default;

double HuMoments::calcMoment(double p, double q)
{
	double sum = 0;
	for (int x = 0; x < width_; x++)
	{
		for (int y = 0; y < height_; y++)
		{
			sum += object_[x + y * width_] * pow(x, p) * pow(y, q);
		}
	}
	return sum;
}

void HuMoments::calcOrgins()
{
	orginOfX_ = calcMoment(1, 0) / calcMoment(0, 0);
	orginOfY_ = calcMoment(0, 1) / calcMoment(0, 0);
}

double HuMoments::calcOrginMoment(double p, double q)
{
	double sum = 0;
	for (int x = 0; x < width_; x++)
	{
		for (int y = 0; y < height_; y++)
		{
			sum += object_[x + y * width_] * pow((x - orginOfX_), p) * pow((y - orginOfY_), q);
		}
	}
	return sum;
}

double HuMoments::calcNormalizedMoment(double p, double q)
{
	double temp = (p + q) / 2 + 1;
	return calcOrginMoment(p, q) / pow(calcOrginMoment(0, 0), temp);
}

void HuMoments::calcInvariantMoments()
{
	huMoments_[0] = calcNormalizedMoment(2, 0)
		+ calcNormalizedMoment(0, 2);

	huMoments_[1] = pow((calcNormalizedMoment(2, 0) - calcNormalizedMoment(0, 2)), 2)
		+ 4 * (pow(calcNormalizedMoment(1, 1), 2));

	huMoments_[2] = pow((calcNormalizedMoment(3, 0) - 3 * calcNormalizedMoment(1, 2)), 2)
		+ pow((3 * calcNormalizedMoment(2, 1) - calcNormalizedMoment(0, 3)), 2);

	huMoments_[3] = pow((calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2)), 2)
		+ pow((calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3)), 2);

	huMoments_[4] = (calcNormalizedMoment(3, 0) - 3 * calcNormalizedMoment(1, 2))
		* (calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2))
		* (pow(calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2), 2)
			- 3 * pow(calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3), 2))
		+ (3 * calcNormalizedMoment(2, 1) - calcNormalizedMoment(0, 3))
		* (calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3))
		* (3 * pow(calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2), 2)
			- pow(calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3), 2));

	huMoments_[5] = (calcNormalizedMoment(2, 0) - calcNormalizedMoment(0, 2))
		* (pow(calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2), 2)
			- pow(calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3), 2))
		+ (4 * calcNormalizedMoment(1, 1)
			* (calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2))
			* (calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3)));

	huMoments_[6] = (3 * calcNormalizedMoment(2, 1) - calcNormalizedMoment(0, 3))
		* (calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2))
		* (pow(calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2), 2)
			- 3 * pow(calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3), 2))
		- (calcNormalizedMoment(3, 0) - 3 * calcNormalizedMoment(1, 2))
		* (calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3))
		* (3 * pow(calcNormalizedMoment(3, 0) + calcNormalizedMoment(1, 2), 2)
			- pow(calcNormalizedMoment(2, 1) + calcNormalizedMoment(0, 3), 2));
}

double* HuMoments::getHuMoments()
{
	return huMoments_;
}

void HuMoments::logTransform()
{
	for (int i = 0; i < 7; i++)
	{
		huMoments_[i] = -1 * copysign(1.0, huMoments_[i]) * log10(fabs(huMoments_[i]));
	}
}
