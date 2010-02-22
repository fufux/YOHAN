
#include "Tetrahedron.h"
#include "Util.h"
#include "Volume.h"

Tetrahedron::Tetrahedron(int id, Volume* volume, vector<Point*> points)
{
	this->id = id;
	this->volume = volume;
	this->points = points;

	// compute mass
	mass = abs(util::crossAndDotProd(points[0]->getU(), points[1]->getU(), points[2]->getU(), points[3]->getU()))/6;

	// compute beta
	beta = new double*[3];
	for (int i=0; i<3; i++)
		beta[i] = new double[3];

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

	for(int i=0;i<3;i++){
		delete []a[i];
	}
	delete[] a;

	// compute core jacobian
	double** n = new double*[4];
	for (int i=0; i<4; i++)
		n[i] = new double[3];
	
	if(util::crossAndDotProd(points[1]->getX(),points[2]->getX(),points[3]->getX(),points[0]->getX())<0){
		n[0] = util::crossProd(points[1]->getX(),points[2]->getX(),points[3]->getX());
	}else{
		n[0] = util::crossProd(points[1]->getX(),points[3]->getX(),points[2]->getX());
	}
	if(util::crossAndDotProd(points[0]->getX(),points[2]->getX(),points[3]->getX(),points[1]->getX())<0){
		n[1] = util::crossProd(points[0]->getX(),points[2]->getX(),points[3]->getX());
	}else{
		n[1] = util::crossProd(points[0]->getX(),points[3]->getX(),points[2]->getX());
	}
	if(util::crossAndDotProd(points[0]->getX(),points[1]->getX(),points[3]->getX(),points[2]->getX())<0){
		n[2] = util::crossProd(points[0]->getX(),points[1]->getX(),points[3]->getX());
	}else{
		n[2] = util::crossProd(points[0]->getX(),points[3]->getX(),points[1]->getX());
	}
	if(util::crossAndDotProd(points[0]->getX(),points[1]->getX(),points[2]->getX(),points[3]->getX())<0){
		n[3] = util::crossProd(points[0]->getX(),points[1]->getX(),points[2]->getX());
	}else{
		n[3] = util::crossProd(points[0]->getX(),points[2]->getX(),points[1]->getX());
	}
	
	
	// initialise ----
	coreJacobian = new double**[16];
	for(int i=0;i<16;i++){
		coreJacobian[i] = new double*[3];
	}
	for(int i=0;i<16;i++){
		for(int j=0;j<3;j++){
			coreJacobian[i][j] = new double[3];
		}
	}
	for(int i=0;i<16;i++){
		for(int j=0;j<3;j++){
			for(int k=0;k<3;k++){
				coreJacobian[i][j][k]=0;
			}
		}
	}
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			// ---------------
			util::prodVecTransVec(coreJacobian,i*4+j,-volume->getMaterial()->lambda,n[i],n[j]);
			util::scalVecVecId(coreJacobian,i*4+j,-volume->getMaterial()->mu,n[i],n[j]);
			util::prodVecTransVec(coreJacobian,i*4+j,-volume->getMaterial()->mu,n[j],n[i]);
		}
	}

	delete[] n;
	
}

Tetrahedron::~Tetrahedron(void)
{
	for(int i=0;i<16;i++){
		for(int j=0;j<3;j++){
			delete []coreJacobian[i][j];
		}
	}
	for(int i=0;i<16;i++){
		delete []coreJacobian[i];
	}
	delete []coreJacobian;

	for(int i=0;i<3;i++){
		delete []beta[i];
	}
	delete []beta;
}

vector<Point*> Tetrahedron::getPoints()
{
	return points;
}

double** Tetrahedron::getBeta()
{
	return beta;
}

double*** Tetrahedron::getCoreJacobian()
{
	return coreJacobian;
}

double Tetrahedron::getMass()
{
	return mass;
}