
#include "Util.h"

using namespace util;

double util::det3(double** a){
	return a[0][0]*a[1][1]*a[2][2]+a[1][0]*a[2][1]*a[0][2]+a[2][0]*a[0][1]*a[1][2]
	-a[0][2]*a[1][1]*a[2][0]-a[1][2]*a[2][1]*a[0][0]-a[2][2]*a[0][1]*a[1][0];
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

double util::norm(double* x)
{
	return sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
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

void util::polarDecomposition(double** x_plus, double** x, double** inv)
{	
	for(int n=0;n<10000;n++){
		util::inv(inv, x);
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				x_plus[i][j] = (x[i][j]-inv[j][i])/2;
			}
		}
		if(normMinus(x_plus,x)/norm(x) < 1e-14){
			cout << n << endl;
			break;
		}
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				x[i][j] = x_plus[i][j];
			}
		}
	}
}



/*
void util::exchange_row(double** M, int k, int l, int m, int n) {
	if (k<=0 || l<=0 || k>n || l>n || k==l)
		return;
	double tmp;
	for (int j=0; j<n; j++)
	{
		tmp = M[k-1][j];
		M[k-1][j] = M[l-1][j];
		M[l-1][j] = tmp;
	}	
}



bool util::vectorIntContains(vector<int>* v, int e)
{
	for (int i=0; i < v->size(); i++)
	{
		if ((*v)[i] == e) return true;
	}
	return false;
}


bool util::inv(double** B, double** M, int m, int n) {

	//Pour stocker les lignes pour lesquels un pivot a déjà été trouvé
	vector<int> I;

	//Pour stocker les colonnes pour lesquels un pivot a déjà été trouvé
	vector<int> J;

	//Pour calculer l'inverse de la matrice initiale
	double** A = new double*[m];
	for (int i=0; i<m; i++)
		A[i] = new double[n];

	//Copie de M dans A et Mise en forme de B : B=I
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			A[i][j] = M[i][j];
			if (i==j)
				B[i][j] = 1;
			else 
				B[i][j] = 0;
		}
	}

	//Paramètres permettant l'arrêt prématuré des boucles ci-dessous si calcul impossible	
	bool bk = true;
	bool bl = true;

	//Paramètres de contrôle pour la recherche de pivot
	int cnt_row = 0;
	int cnt_col = 0;

	//paramètre de stockage de coefficients
	double a, tmp;	

	for (int k=0; k<n && bk; k++) 
	{
		if (!vectorIntContains(&I, k))
		{
			I.push_back(k);
			cnt_row++;
			bl = true;
			for (int l=0; l<n && bl; l++) 
			{
				if (!vectorIntContains(&J, l)) 
				{
					a = A[k][l]; 			
					if (a != 0) 
					{
						J.push_back(l);
						cnt_col++;			    
						bl = false; //permet de sortir de la boucle car le pivot a été trouvé
						for (int p=0; p<n; p++)
						{
							if (p != k)
							{
								tmp = A[p][l];
								for (int q=0; q<n; q++)
								{
									A[p][q] = A[p][q] - A[k][q]*(tmp/a);
									B[p][q] = B[p][q] - B[k][q]*(tmp/a);
								}
							}	
						}
					}			
				}
			}
			if (cnt_row != cnt_col) 
			{
				//Matrix is singular";
				//Pas de pivot possible, donc pas d'inverse possible! On sort de la boucle
				bk = false;
				k = n; 
			}	       
		}
	}

	if (!bk)
	{
		//Le pivot n'a pas pu être trouve précédemment, ce qui a donne bk = false
		cout << "Matrix is singular" << endl;
		for (int i=0; i<n; i++) {
			for (int j=0; j<n; j++) {
				B[j][i] = M[j][i];
			}
		}
		for (int i=0; i<m; i++)
			delete []A[i];
		delete []A;
		return false;
	}
	else 
	{
		//Réorganisation des colonnes de sorte que A=I et B=Inv(M). Méthode de Gauss-Jordan
		for (int l=0; l<n; l++)
		{
			for (int k=0; k<n; k++)
			{
				a = A[k][l];
				if (a != 0)
				{
					A[k][l] = 1;
					for (int p=0; p<n; p++)
					{
						B[k][p] = B[k][p]/a;
					}
					if (k != l)
					{
						exchange_row(A,k+1,l+1,n,n);
						exchange_row(B,k+1,l+1,n,n);
					}
					k = n; //Pour sortir de la boucle car le coefficient non nul a été trouve
				}
			}
		}
		for (int i=0; i<m; i++)
			delete []A[i];
		delete []A;
		return true;	
	}	
}
*/