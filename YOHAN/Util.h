#pragma once

#include "stdafx.h"
#include <Eigen/Eigen>

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN

namespace util
{
	void log(const char* message);
	void log(const std::string message);
	std::string ws2s(const std::wstring& s);
	double norm(double** x);
	double normMinus(double** x, double** y);
	void polarDecomposition(Matrix3d* x);
}
