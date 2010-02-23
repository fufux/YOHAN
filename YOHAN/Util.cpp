#include <Eigen/Eigen>
#include "Util.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN
using namespace util;


void util::log(const char* message)
{
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
}

void util::log(const std::string message)
{
	util::log(message.c_str());
}

std::string util::ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;
}

double util::norm(double** x)
{
	double norm = 0;
	double max;
	for( int i=0;i<3;i++){
		max = 0;
		for(int j=0;j<3;j++){
			max = abs(x[i][j])>max?abs(x[i][j]):max;
		}
		norm += max;
	}
	return norm;
}

double util::normMinus(double** x, double** y)
{
	double norm = 0;
	double max;
	for( int i=0;i<3;i++){
		max = 0;
		for(int j=0;j<3;j++){
			max = abs(x[i][j]-y[i][j])>max?abs(x[i][j]-y[i][j]):max;
		}
		norm += max;
	}
	return norm;
}

void util::polarDecomposition(Matrix3d* x)
{
	Eigen::SVD<Matrix3d> svd;
	svd.compute(*x);
	*x = svd.matrixU() * svd.matrixV().adjoint();
}
