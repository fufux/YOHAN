#pragma once

#include "stdafx.h"

namespace util
{
	//double det3(double** a);
	double crossAndDotProd(double* p0, double* p1, double* p2, double* p3);
	double* crossProd(double* p0, double* p1, double* p2);
	double dotProd(double* p0, double* p1, double* p2);
	std::string ws2s(const std::wstring& s);
	void prodVecTransVec(double*** core, int ind,double k, double* n, double* nt);
	void scalVecVecId(double*** core, int ind, double k, double* n, double* nt);

	// r is the result and must be allocated before !
	void matrixProd(double** r, double** a, double** b);
	// r is the result and must be allocated before !
	void matrixProdTrans(double** r, double** a, double** b);
	// inv is output, a is input, they must be allocated before
	//void exchange_row(double** M, int k, int l, int m, int n);
	//bool vectorIntContains(vector<int>* v, int e);
	//void inv(double** B, double** M);
	double norm(double** x);
	double normMinus(double** x, double** y);
	// x is the in/out and old_x, inv must be allocated at double 3x3
	void polarDecomposition(double** x, double** old_x, double** inv);

	void inv( double **Y, double **A);
	double det3( double **mat);
}
