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
	double norm(double* x);
	double norm(double** x);
	double normMinus(double** x, double** y);

	inline void polarDecomposition(Matrix3d& f, Matrix3d& q)
	{
		/*q.col(0) = f.col(0)/f.col(0).norm();
		q.col(1) = (f.col(1)-q.col(0).dot(f.col(1))*q.col(1));
		q.col(1) = q.col(1)/q.col(1).norm();
		q.col(2) = q.col(0).cross(q.col(1));*/

		Eigen::SVD<Matrix3d> svd;
		svd.compute(f);
		q = svd.matrixU() * svd.matrixV().adjoint();
	}
}
