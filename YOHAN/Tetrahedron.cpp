
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
	v1 = v1.cross(v2).eval();
	v2(0) = u4[0]-u1[0];
	v2(1) = u4[1]-u1[1];
	v2(2) = u4[2]-u1[2];
	vol = abs( v1.dot(v2) ) / 6;
	mass = volume->getMaterial()->rho * vol;

	// compute beta
	computeBeta();

	// initialise ----
	for(int i=0;i<16;i++){
		Matrix3d m;
		coreJacobian.push_back(m);
	}

	// compute core jacobian
	computeCoreJacobian();
}

Tetrahedron::Tetrahedron()
{
	this->id = -1;
}

Tetrahedron::~Tetrahedron(void)
{
}

vector<Point*>& Tetrahedron::getPoints()
{
	return points;
}

Matrix3d& Tetrahedron::getBeta()
{
	return beta;
}

vector<Matrix3d>& Tetrahedron::getCoreJacobian()
{
	return coreJacobian;
}

double Tetrahedron::getMass()
{
	return mass;
}

int Tetrahedron::getID()
{
	return id;
}

Volume* Tetrahedron::getVolume()
{
	return volume;
}

Vector3d& Tetrahedron::getN(int i)
{
	return n[i];
}

double Tetrahedron::getVol()
{
	return vol;
}

void Tetrahedron::computeBeta()
{
	double* u1 = points[0]->getU();
	double* u2 = points[1]->getU();
	double* u3 = points[2]->getU();
	double* u4 = points[3]->getU();
	beta(0,0) = u2[0]-u1[0];
	beta(1,0) = u2[1]-u1[1];
	beta(2,0) = u2[2]-u1[2];
	beta(0,1) = u3[0]-u1[0];
	beta(1,1) = u3[1]-u1[1];
	beta(2,1) = u3[2]-u1[2];
	beta(0,2) = u4[0]-u1[0];
	beta(1,2) = u4[1]-u1[1];
	beta(2,2) = u4[2]-u1[2];
	beta = beta.inverse().eval();
}

void Tetrahedron::computeN()
{
	Vector3d v1,v2;

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
}


void Tetrahedron::computeCoreJacobian()
{
	computeN();
	/*
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			// ---------------
			coreJacobian[i*4+j] = (-volume->getMaterial()->lambda) * (n[i] * n[j].transpose());

			for (int ii=0;ii<3;ii++)
				coreJacobian[i*4+j](ii,ii) += (-volume->getMaterial()->mu) * n[i](ii) * n[j](ii);

			coreJacobian[i*4+j] += (-volume->getMaterial()->mu) * (n[j] * n[i].transpose());
		}
	}*/

	Vector3d y[4];
	Matrix3d m1,m2,m3,m4,m5,m6;

	double v = getVol();

	y[1] = beta.row(0);
	y[2] = beta.row(1);
	y[3] = beta.row(2);
	y[0] = -y[1]-y[2]-y[3];
	
	

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			m1 = Matrix3d::Zero();
			m2 = Matrix3d::Zero();
			m3 = Matrix3d::Zero();
			m4 = Matrix3d::Zero();
			m5 = Matrix3d::Zero();
			m6 = Matrix3d::Zero();
			m1(0,0) = y[i].x();
			m1(1,1) = y[i].y();
			m1(2,2) = y[i].z();

			m3(0,0) = y[j].x();
			m3(1,1) = y[j].y();
			m3(2,2) = y[j].z();

			m4(0,0) = y[i].y();
			m4(0,2) = y[i].z();
			m4(1,0) = y[i].x();
			m4(1,1) = y[i].z();
			m4(2,1) = y[i].y();
			m4(2,2) = y[i].x();

			m6(0,0) = y[j].y();
			m6(0,1) = y[j].x();
			m6(1,1) = y[j].z();
			m6(1,2) = y[j].y();
			m6(2,0) = y[j].z();
			m6(2,2) = y[j].x();

			double a = v*(volume->getMaterial()->lambda + 2*volume->getMaterial()->mu);
			double b = v*volume->getMaterial()->lambda;
			double c = 2*v*volume->getMaterial()->mu;

			m2 = b * Matrix3d::Ones();
			m2(0,0) = a;
			m2(1,1) = a;
			m2(2,2) = a;

			m5 = c * Matrix3d::Identity();

			coreJacobian[i*4+j] = m1*m2*m3 + m4*m5*m6;

		}
	}

}


/* Add by Ning, for fracture */
void Tetrahedron::setStress(Matrix3d& stress)
{
	this->stress = stress;
}

void Tetrahedron::setQ(Matrix3d& q)
{
	this->Q = q;
}

Matrix3d& Tetrahedron::getQ()
{
	return this->Q;
}

void Tetrahedron::retrieveEigenOfStress(Matrix<double, 3, 1>& eigenValue, Matrix3d& eigenVector)
{
	util::retrieveEigen(this->stress, eigenValue, eigenVector);
}

void Tetrahedron::setTensileForce(int index, Matrix<double, 3, 1>& force)
{
	this->tensileForce[index] = force;
}

void Tetrahedron::setCompressiveForce(int index, Matrix<double, 3, 1>& force)
{
	this->compressiveForce[index] = force;
}

Matrix<double, 3, 1>& Tetrahedron::getTensileForce(int index)
{
	return this->tensileForce[index];
	
}

Matrix<double, 3, 1>& Tetrahedron::getCompressiveForce(int index)
{
	return this->compressiveForce[index];
}