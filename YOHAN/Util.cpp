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

/*
NOTE:	a. NO intersected point
		b. Only one point, different from P1 P2
		c. P1 P2 is the intersected point, or the line is parallel with the plane
*/

/*
int util::intersect_line_plane(double* p1, double* p2, Matrix<double, 3, 1>& normalOfPlane, double* pOnPlane, double& ratio)
{
	double u[3] = {p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};					// u = p2 - p1

	double w[3] = {pOnPlane[0] - p1[0], pOnPlane[1] - p1[1], pOnPlane[2] - p1[2]};	// w = p1 - pOnPlane

	double D = normalOfPlane(0,0) * u[0] + normalOfPlane(1,0) * u[1] + normalOfPlane(2,0) * u[2];		// D = Dot(pNormalofPlane, u)

	double N = -(normalOfPlane(0,0) * w[0] + normalOfPlane(1,0) * w[1] + normalOfPlane(2,0) * w[2]);	// N = -Dot(pNormalofPlane, w)

	if (fabs(D) < 1E-20)	// epsilonP : the parallel
	{
		if (fabs(N) < 1E-40)	// epsilonN : P1 is on the plane
		{
			//test if P2 is also on the plane
			double wEx[3] = {pOnPlane[0] - p2[0], pOnPlane[1] - p2[1], pOnPlane[2] - p2[2]};
			double NEx = -(normalOfPlane(0,0) * wEx[0] + normalOfPlane(1,0) * wEx[1] + normalOfPlane(2,0) * wEx[2]);

			if (fabs(NEx) < 1E-40)	// epsilonNEx : P2 is on the plane
				return 100;	// the line is on the plane
			else
			{
				//pIntersect = p1
				ratio = 0;

				return 3;		//pIntersect = p1
			}

		}
		else
			return 0;	// no intersect point
	}

	ratio = N / D;

	// test if this point is different from P2
	double eps = fabs(ratio - 1);
	if (eps < 1E-40)	// epsilonNEx : P2 is the intersect point
		return 10;	// pIntersect = p2
	else
		return 1;	// Only one point, different from P1 P2
}
*/


/*
NOTE:	a. NO intersected point
		b. Only one point, different from P1 P2
		c. P1 P2 is the intersected point, or the line is parallel with the plane
*/
int util::intersect_line_plane(double* p1, double* p2, Matrix<double, 3, 1>& normalOfPlane, double* pOnPlane, double& ratio)
{
	double u[3] = {p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};					// u = p2 - p1

	double w[3] = {p1[0] - pOnPlane[0], p1[1] - pOnPlane[1], p1[2] - pOnPlane[2]};	// w = p1 - pOnPlane

	// normalize
	double normU = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	double normW = sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
	if (normU > 1E-40)
	{
		u[0] /= normU;u[1] /= normU;u[2] /= normU;
	}
	else
	{
		u[0] = 0;u[1] = 0;u[2] = 0;
	}

	if (normW > 1E-40)
	{
		w[0] /= normW;w[1] /= normW;w[2] /= normW;
	}
	else
	{
		w[0] = 0;w[1] = 0;w[2] = 0;
	}

	double D = normalOfPlane(0,0) * u[0] + normalOfPlane(1,0) * u[1] + normalOfPlane(2,0) * u[2];		// D = Dot(pNormalofPlane, u)

	double N = -(normalOfPlane(0,0) * w[0] + normalOfPlane(1,0) * w[1] + normalOfPlane(2,0) * w[2]);	// N = -Dot(pNormalofPlane, w)

	if (fabs(D) < 1E-20)	// epsilonP : the parallel
	{
		if (fabs(N) < 1E-40)	// epsilonN : P1 is on the plane
		{
			//test if P2 is also on the plane
			double wEx[3] = {pOnPlane[0] - p2[0], pOnPlane[1] - p2[1], pOnPlane[2] - p2[2]};
			double NEx = -(normalOfPlane(0,0) * wEx[0] + normalOfPlane(1,0) * wEx[1] + normalOfPlane(2,0) * wEx[2]);

			if (fabs(NEx) < 1E-40)	// epsilonNEx : P2 is on the plane
				return 100;	// the line is on the plane
			else
			{
				//pIntersect = p1
				ratio = 0;

				return 3;		//pIntersect = p1
			}

		}
		else
			return 0;	// no intersect point (of the line)
	}

	ratio = N / D;

	// test if this point is different from P2
	double eps = fabs(ratio - 1);
	if (eps < 1E-40)	// epsilonNEx : P2 is the intersect point
		return 10;	// pIntersect = p2
	else
	{
		if (ratio < 1 && ratio > 0)
			return 1;	// Only one point, different from P1 P2
		else
			return 0;	// no intersect point (of the section)
	}
}
