#include "HMath.h"


HMath::HMath(void)
{
}


HMath::~HMath(void)
{
}

float HMath::Clamp(float val, float min, float max)
{
	if (val < min) 
	{
		return min;
	}
	if (val > max)
	{
		return max;
	}
	return val;
}