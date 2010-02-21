
#include "Util.h"

using namespace util;

double* util::prodVect(double* a1, double* b1, double* a2, double* b2)
{
	double* result = new double[3];
	double x1,x2,y1,y2,z1,z2;
	x1 = a1[0] - b1[0];
	y1 = a1[1] - b1[1];
	z1 = a1[2] - b1[2];
	x2 = a2[0] - b2[0];
	y2 = a2[1] - b2[1];
	z2 = a2[2] - b2[2];
	result[0] = y1*z2 - y2*z1;
	result[1] = z1*x2 - z2*x1;
	result[2] = x1*y2 - x2*y1;
	return result;
}
