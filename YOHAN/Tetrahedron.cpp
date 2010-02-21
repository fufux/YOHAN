
#include "Tetrahedron.h"
#include "Util.h"

Tetrahedron::Tetrahedron(int id, vector<Point*> points)
{
	this->id = id;
	this->points = points;

	// compute mass
	mass = abs(util::crossAndDotProd(points[0]->getU(), points[1]->getU(), points[2]->getU(), points[3]->getU()))/6;

	// compute beta
	double* u1 = points[0]->getU();
	double* u2 = points[1]->getU();
	double* u3 = points[2]->getU();
	double* u4 = points[3]->getU();

	double** a = new double*[3];
	for (int i=0; i<3; i++)
		a[i] = new double[3];
	a[0][0] = u2[0]-u1[0];
	a[1][0] = u2[1]-u1[1];
	a[2][0] = u2[2]-u1[2];
	a[0][1] = u3[0]-u1[0];
	a[1][1] = u3[1]-u1[1];
	a[2][1] = u3[2]-u1[2];
	a[0][2] = u4[0]-u1[0];
	a[1][2] = u4[1]-u1[1];
	a[2][2] = u4[2]-u1[2];

	double det = util::det3(a);

	beta[0][0] = (a[1][1]*a[2][2]-a[1][2]*a[2][1])/det;
	beta[1][0] = (a[1][2]*a[2][0]-a[1][0]*a[2][2])/det;
	beta[2][0] = (a[1][0]*a[2][1]-a[1][1]*a[2][0])/det;
	beta[0][1] = (a[0][2]*a[2][1]-a[0][1]*a[2][2])/det;
	beta[1][1] = (a[0][0]*a[2][2]-a[0][2]*a[2][0])/det;
	beta[2][1] = (a[0][1]*a[2][0]-a[0][0]*a[2][1])/det;
	beta[0][2] = (a[0][1]*a[1][2]-a[0][2]*a[1][1])/det;
	beta[1][2] = (a[0][2]*a[1][0]-a[0][0]*a[1][2])/det;
	beta[2][2] = (a[0][0]*a[1][1]-a[0][1]*a[1][0])/det;

	delete[] a;

	// compute force jacobian
	//forceJacobian = new double[16][3][3];

}

Tetrahedron::~Tetrahedron(void)
{
}

vector<Point*> Tetrahedron::getPoints()
{
	return points;
}

double** Tetrahedron::getBeta()
{
	return (double**)beta;
}

double*** Tetrahedron::getForceJacobian()
{
	return (double***)forceJacobian;
}

double Tetrahedron::getMass()
{
	return mass;
}