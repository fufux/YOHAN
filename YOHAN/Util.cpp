#include <Eigen/Eigen>
#include "Util.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN
using namespace util;



double util::det3(double** a){
	/*return a[0][0]*a[1][1]*a[2][2]+a[1][0]*a[2][1]*a[0][2]+a[2][0]*a[0][1]*a[1][2]
	-a[0][2]*a[1][1]*a[2][0]-a[1][2]*a[2][1]*a[0][0]-a[2][2]*a[0][1]*a[1][0];*/
	Matrix3d ma;
	for (int i=0; i < 3; i++){
		for (int j=0; j < 3; j++)
			ma << a[i][j];
	}
	return ma.determinant();
}

double util::crossAndDotProd(double* p0, double* p1, double* p2, double* p3){
	double x[3];
	double y[3];
	double z[3];
	for(int i=0;i<3;i++){
		x[i] = p1[i] - p0[i];
		y[i] = p2[i] - p0[i];
		z[i] = p3[i] - p0[i];
	}
	return z[0]*(x[1]*y[2]-x[2]*y[1]) + z[1]*(x[2]*y[0]-x[0]*y[2]) + z[2]*(x[0]*y[1]-x[1]*y[0]);
}

double* util::crossProd(double* p0, double* p1, double* p2){
	double* n = new double[3];
	double x[3];
	double y[3];
	for(int i=0;i<3;i++){
		x[i] = p1[i] - p0[i];
		y[i] = p2[i] - p0[i];
	}
	n[0] = x[1]*y[2]-x[2]*y[1];
	n[1] = x[2]*y[0]-x[0]*y[2];
	n[2] = x[0]*y[1]-x[1]*y[0];
	return n;
}

double util::dotProd(double* p0, double* p1, double* p2){
	return (p1[0]-p0[0])*(p2[0]-p0[0]) + (p1[1]-p0[1])*(p2[1]-p0[1]) + (p1[2]-p0[2])*(p2[2]-p0[2]);
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

void util::prodVecTransVec(double*** core,int ind, double k, double* n, double* nt)
{
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			core[ind][i][j]+=k*n[i]*nt[j];
		}
	}
}

void util::scalVecVecId(double*** core, int ind, double k, double* n, double* nt)
{
	for(int i=0;i<3;i++)
		core[ind][i][i] += k*n[i]*nt[i];
}

// r is the result and must be allocated before !
void util::matrixProd(double** r, double** a, double** b)
{
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			r[i][j] = 0;
			for(int k=0;k<3;k++)
				r[i][j] += a[i][k]*b[k][j];
		}
	}
}

// r is the result and must be allocated before !
void util::matrixProdTrans(double** r, double** a, double** b)
{
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			r[i][j] = 0;
			for(int k=0;k<3;k++)
				r[i][j] += a[i][k]*b[j][k];
		}
	}
}

void util::inv(double** inv, double** a)
{
	double det = util::det3(a);

	inv[0][0] = (a[1][1]*a[2][2]-a[1][2]*a[2][1])/det;
	inv[1][0] = (a[1][2]*a[2][0]-a[1][0]*a[2][2])/det;
	inv[2][0] = (a[1][0]*a[2][1]-a[1][1]*a[2][0])/det;
	inv[0][1] = (a[0][2]*a[2][1]-a[0][1]*a[2][2])/det;
	inv[1][1] = (a[0][0]*a[2][2]-a[0][2]*a[2][0])/det;
	inv[2][1] = (a[0][1]*a[2][0]-a[0][0]*a[2][1])/det;
	inv[0][2] = (a[0][1]*a[1][2]-a[0][2]*a[1][1])/det;
	inv[1][2] = (a[0][2]*a[1][0]-a[0][0]*a[1][2])/det;
	inv[2][2] = (a[0][0]*a[1][1]-a[0][1]*a[1][0])/det;

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
	cout << "X:" << endl << *x << endl;
	svd.compute(*x);
	cout << "SVD" << endl;
	*x = svd.matrixU() * svd.matrixV().adjoint();
	cout << "X:" << endl << *x << endl;
}
