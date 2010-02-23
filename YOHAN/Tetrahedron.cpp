
#include "Tetrahedron.h"
#include "Util.h"
#include "Volume.h"

Tetrahedron::Tetrahedron(int id, Volume* volume, vector<Point*> points)
{
	this->id = id;
	this->volume = volume;
	this->points = points;
	Vector3d v1,v2;

	double* u1 = points[0]->getU();
	double* u2 = points[1]->getU();
	double* u3 = points[2]->getU();
	double* u4 = points[3]->getU();

	// compute mass
	v1(0) = u2[0]-u1[0];
	v1(1) = u2[1]-u1[1];
	v1(2) = u2[2]-u1[2];
	v2(0) = u3[0]-u1[0];
	v2(1) = u3[1]-u1[1];
	v2(2) = u3[2]-u1[2];
	v1 = v1.cross(v2);
	v2(0) = u4[0]-u1[0];
	v2(1) = u4[1]-u1[1];
	v2(2) = u4[2]-u1[2];
	mass = abs( v1.dot(v2) ) / 6;

	// compute beta
	beta(0,0) = u2[0]-u1[0];
	beta(1,0) = u2[1]-u1[1];
	beta(2,0) = u2[2]-u1[2];
	beta(0,1) = u3[0]-u1[0];
	beta(1,1) = u3[1]-u1[1];
	beta(2,1) = u3[2]-u1[2];
	beta(0,2) = u4[0]-u1[0];
	beta(1,2) = u4[1]-u1[1];
	beta(2,2) = u4[2]-u1[2];
	beta.computeInverse( &beta);
//cout << "beta:" << endl << beta << endl;

	// compute core jacobian
	Vector3d n[4];
	double* x0 = points[0]->getX();
	double* x1 = points[1]->getX();
	double* x2 = points[2]->getX();
	double* x3 = points[3]->getX();

	// ((x2-x1)*(x3-x1)).(x0-x1)
	v1(0) = x2[0]-x1[0];
	v1(1) = x2[1]-x1[1];
	v1(2) = x2[2]-x1[2];
	v2(0) = x3[0]-x1[0];
	v2(1) = x3[1]-x1[1];
	v2(2) = x3[2]-x1[2];
	v1 = v1.cross(v2);
	v2(0) = x0[0]-x1[0];
	v2(1) = x0[1]-x1[1];
	v2(2) = x0[2]-x1[2];
	if (v1.dot(v2) < 0) {
		n[0] = v1;
	} else {
		n[0] = -v1;
	}

	v1(0) = x2[0]-x0[0];
	v1(1) = x2[1]-x0[1];
	v1(2) = x2[2]-x0[2];
	v2(0) = x3[0]-x0[0];
	v2(1) = x3[1]-x0[1];
	v2(2) = x3[2]-x0[2];
	v1 = v1.cross(v2);
	v2(0) = x1[0]-x0[0];
	v2(1) = x1[1]-x0[1];
	v2(2) = x1[2]-x0[2];
	if (v1.dot(v2) < 0) {
		n[1] = v1;
	} else {
		n[1] = -v1;
	}

	v1(0) = x1[0]-x0[0];
	v1(1) = x1[1]-x0[1];
	v1(2) = x1[2]-x0[2];
	v2(0) = x3[0]-x0[0];
	v2(1) = x3[1]-x0[1];
	v2(2) = x3[2]-x0[2];
	v1 = v1.cross(v2);
	v2(0) = x2[0]-x0[0];
	v2(1) = x2[1]-x0[1];
	v2(2) = x2[2]-x0[2];
	if (v1.dot(v2) < 0) {
		n[2] = v1;
	} else {
		n[2] = -v1;
	}

	v1(0) = x1[0]-x0[0];
	v1(1) = x1[1]-x0[1];
	v1(2) = x1[2]-x0[2];
	v2(0) = x2[0]-x0[0];
	v2(1) = x2[1]-x0[1];
	v2(2) = x2[2]-x0[2];
	v1 = v1.cross(v2);
	v2(0) = x3[0]-x0[0];
	v2(1) = x3[1]-x0[1];
	v2(2) = x3[2]-x0[2];
	if (v1.dot(v2) < 0) {
		n[3] = v1;
	} else {
		n[3] = -v1;
	}

	double norm = 0;
	for (int i=0; i<4; i++)
		norm += n[i].norm();
	for (int i=0; i<4; i++)
		n[i] /= norm;

	
	// initialise ----
	for(int i=0;i<16;i++){
		Matrix3d m;
		coreJacobian.push_back(m);
	}
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			// ---------------
			coreJacobian[i*4+j] = (-volume->getMaterial()->lambda) * (n[i] * n[j].transpose());

			for (int ii=0;ii<3;ii++)
				coreJacobian[i*4+j](ii,ii) += (-volume->getMaterial()->mu) * n[i](ii) * n[j](ii);

			coreJacobian[i*4+j] += (-volume->getMaterial()->mu) * (n[j] * n[i].transpose());
		}
	}
	
}

Tetrahedron::~Tetrahedron(void)
{
}

vector<Point*> Tetrahedron::getPoints()
{
	return points;
}

Matrix3d Tetrahedron::getBeta()
{
	return beta;
}

vector<Matrix3d> Tetrahedron::getCoreJacobian()
{
	return coreJacobian;
}

double Tetrahedron::getMass()
{
	return mass;
}