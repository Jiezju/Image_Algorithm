#pragma once
#include <cmath>

class HuMoments
{
private:
	int width_, height_;
	double orginOfX_, orginOfY_;
	unsigned char* object_;
	double huMoments_[7];
	bool islog_ = true;

public:
	HuMoments(unsigned char* binaryImage, int width, int height, bool islog);
	~HuMoments();
	double calcMoment(double p, double q);
	void calcOrgins();
	double calcOrginMoment(double p, double q);
	double calcNormalizedMoment(double p, double q);
	void calcInvariantMoments();
	double* getHuMoments();
	void logTransform();
};

