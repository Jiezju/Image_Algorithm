#ifndef AMD_FSR_UTIL_H
#define AMD_FSR_UTIL_H

#include "Point.hpp"

#define MaxAbs(a, b) (std::max(abs(a), abs(b)))

//  (25/16 * (2/5 * x^2 - 1)^2 - (25/16 - 1)) * (1/4 * x^2 - 1)^2
//  |_______________________________________|   |_______________|
//                   base                             window
float Lanczos2(float w, float x2)
{
    // 截断
    x2 = min(x2, w);
    float WindowB = float(2.0 / 5.0) * x2 + float(-1.0);
	float WindowA = w * x2 + float(-1.0);
	WindowB *= WindowB;
	WindowA *= WindowA;
	WindowB = float(25.0 / 16.0) * WindowB + float(-(25.0 / 16.0 - 1.0));
	float Window = (WindowB * WindowA);
}

#endif //AMD_FSR_UTIL_H