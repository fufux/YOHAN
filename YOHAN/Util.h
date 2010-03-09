#pragma once

#include "stdafx.h"
#include <Eigen/Eigen>

class Point;
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
	void plan(vector<Point*>* vertices, int i1, int i2, int i3, Vector3d* n, double* c);
	vector<Vector3d*>* intersect(vector<Vector3d*>* vertices, Vector3d* alpha, double alphaC, Point* pt);
	Vector3d* intersectLinePlan(Vector3d* s, Vector3d* p, Vector3d* alpha, double alphaC);

	inline void polarDecomposition(Matrix3d& f, Matrix3d& q)
	{
		q.col(0) = f.col(0)/f.col(0).norm();
		q.col(1) = (f.col(1)-q.col(0).dot(f.col(1))*q.col(1));
		q.col(1) = q.col(1)/q.col(1).norm();
		q.col(2) = q.col(0).cross(q.col(1));

		/*Eigen::SVD<Matrix3d> svd;
		svd.compute(f);
		q = svd.matrixU() * svd.matrixV().adjoint();*/
	}
}
