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



/* Added by Ning, for fracture */
Matrix3d util::calcul_M33_MA(Matrix<double, 3, 1>& vector)
{
	double norm = vector.norm();

	if (norm < 1E-40)	// consider as 0
	{
		return Matrix3d::Zero();
	}
	else
	{
		Matrix<double, 1, 3> trans = vector.transpose();
		return vector * trans / norm;
	}
}

void util::retrieveEigen(Matrix3d& mat, Matrix<double, 3, 1>& eigenValue, Matrix3d& eigenVector)
{
	/*
	Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
	solver.compute(mat, true);

	eigenValue = solver.eigenvalues();
	eigenVector = solver.eigenvectors();
	*/
	
	// see if zero matrix, because EigenSolver can not solve this matrix
	if (mat.isZero())
	{
		eigenValue = Matrix<double, 3, 1>::Zero();
		eigenVector = Matrix3d::Zero();
	}
	else
	{	
		Eigen::EigenSolver<Matrix3d> solver;
		solver.compute(mat);

		Matrix<std::complex<double>, 3, 1> tmpValue = solver.eigenvalues();
		Matrix<std::complex<double>, 3, 3> tmpVector = solver.eigenvectors();

		// copy without image ( or its norm )
		for (int i = 0; i < 3; i++)
		{
			eigenValue(i,0) = tmpValue(i,0).real();
			
			eigenVector(i,0) = tmpVector(i,0).real();
			eigenVector(i,1) = tmpVector(i,1).real();
			eigenVector(i,2) = tmpVector(i,2).real();

		}
	}
	

	//could check if the eigen is correct by its image number
}
