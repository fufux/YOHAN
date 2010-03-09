
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
	Eigen::LU<Matrix3d> lu(beta);
	beta = lu.inverse();
	//beta = beta.inverse().eval();
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
	/*for(int i=0;i<4;i++){
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
	y[0] = (-y[1]-y[2]-y[3]);
	
	

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

void Tetrahedron::remesh(Point* orginal, Point* replica, Matrix<double, 3, 1>& nvector, vector<Point*>& pointList)
{
	/* assume that the given point is P0, the rest is P1, P2 and P3 */
	Point *p0 = orginal, *p1, *p2, *p3;
	
	if (orginal == points[0])
	{
		p1 = points[1];
		p2 = points[2];
		p3 = points[3];
	}
	else if (orginal == points[1])
	{
		p1 = points[0];
		p2 = points[2];
		p3 = points[3];
	}
	else if (orginal == points[2])
	{
		p1 = points[1];
		p2 = points[0];
		p3 = points[3];
	}
	else if (orginal == points[3])
	{
		p1 = points[1];
		p2 = points[2];
		p3 = points[0];
	}
	else
	{
		// must not reach here, it is a fetal error
		util::log("FETAL ERROR IN Tetrahedron::Remesh");
	}
	

	/* calculate the intersected points for P1P2 and P1P3

	NOTE:	a. 2 new points different from P1 P2 and P3
			b. 1 new point, and another is same to P1 P2 or P3
			c. no new point :
				c1. P1P2
				c2. P1P3
				c3. P2P3
	*/

	double ratioP1P2, ratioP1P3, ratioP2P3;
	
	int resP1P2 = util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
	int resP1P3 = util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
	int resP2P3 = util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);

	int state = -1;		// 0 - a, 1 - b, 2 - c

	/* 2 New Points, assert that p4:p1-p2, p5:p1-p3 */
	if (resP1P2 + resP1P3 + resP2P3 == 2)
	{
		state = 0;

		// swap the p1 p2 p3 to keep the assertion
		if (resP1P2 == 0)	//1<->3
		{
			Point* tmp = p1;
			p1 = p3;
			p3 = tmp;

			// update ratio
			util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
			util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
			util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
		}
		else if (resP1P3 == 0)	// 1<->2
		{			
			Point* tmp = p1;
			p1 = p2;
			p2 = tmp;

			// update ratio
			util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
			util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
			util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
		}
		else
		{
			// order is correct
		}
	}
	/* 1 New Points, assert that p4:p2-p3 */
	else if ((resP1P2 == 3 && resP1P3 == 3 && resP2P3 == 1) ||
			 (resP1P2 == 3 && resP1P3 == 0 && resP2P3 == 1) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 1))		// 3,3,1 == 3,0,1 == 0,3,1
	{
		state = 1;
		// order is correct
	}
	else if ((resP1P2 == 1 && resP1P3 == 10 && resP2P3 == 10) ||
			 (resP1P2 == 1 && resP1P3 == 10 && resP2P3 == 0) ||
			 (resP1P2 == 1 && resP1P3 == 0 && resP2P3 == 10))		// 1,10,10 == 1,0,10 == 1,10,0
	{
		state = 1;
		//1<->3
		Point* tmp = p1;
		p1 = p3;
		p3 = tmp;

		// update ratio
		util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
		util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
		util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
	}
	else if ((resP1P2 == 10 && resP1P3 == 1 && resP2P3 == 3) ||
			 (resP1P2 == 10 && resP1P3 == 1 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 1 && resP2P3 == 3))		// 10,1,3 == 10,1,0 == 0,1,3
	{
		state = 1;
		// 1<->2
		Point* tmp = p1;
		p1 = p2;
		p2 = tmp;

		// update ratio
		util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
		util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
		util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
	}
	else if ((resP1P2 == 3 && resP1P3 == 3 && resP2P3 == 0) ||
			 (resP1P2 == 10 && resP1P3 == 0 && resP2P3 == 3) ||
			 (resP1P2 == 1 && resP1P3 == 0 && resP2P3 == 3) ||
			 (resP1P2 == 3 && resP1P3 == 0 && resP2P3 == 10) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 1) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 10) ||
			 (resP1P2 == 10 && resP1P3 == 0 && resP2P3 == 1) ||
			 (resP1P2 == 3 && resP1P3 == 1 && resP2P3 == 0) ||
			 (resP1P2 == 10 && resP1P3 == 10 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 3) ||
			 (resP1P2 == 0 && resP1P3 == 1 && resP2P3 == 10) ||
			 (resP1P2 == 1 && resP1P3 == 3 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 3) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 10))
	{
		// ignore
	}
	else
	{
		util::log("CASE NOT SOLVED: Tetrahedron::remesh");
	}

	if (state == 0)	/* 2 New Points, assert that p4:p1-p2, p5:p1-p3 */
	{
		/*DEBUG ONLY
		ratioP1P2 = 0.5;
		ratioP1P3 = 0.5;
		END DEBUG*/

		/* create the 2 new points */
		double *x1 = p1->getX(), *v1 = p1->getV(), *u1 = p1->getU();
		double *x2 = p2->getX(), *v2 = p2->getV(), *u2 = p2->getU();
		double *x3 = p3->getX(), *v3 = p3->getV(), *u3 = p3->getU();

		// np1 : between P1 and P2, np1 = p1 + t * (p2 - p1)
		// np2 : between P1 and P3, np2 = p1 + t * (p3 - p1)
		double xP1P2[3], vP1P2[3], uP1P2[3];
		double xP1P3[3], vP1P3[3], uP1P3[3];

		for (int i = 0; i < 3; i++)
		{
			xP1P2[i] = x1[i] + ratioP1P2 * (x2[i] - x1[i]);
			vP1P2[i] = v1[i] + ratioP1P2 * (v2[i] - v1[i]);
			uP1P2[i] = u1[i] + ratioP1P2 * (u2[i] - u1[i]);

			xP1P3[i] = x1[i] + ratioP1P3 * (x3[i] - x1[i]);
			vP1P3[i] = v1[i] + ratioP1P3 * (v3[i] - v1[i]);
			uP1P3[i] = u1[i] + ratioP1P3 * (u3[i] - u1[i]);
		}

		Point* p4 = new Point(pointList.size(), xP1P2, vP1P2, uP1P2, true);
		pointList.push_back(p4);
		Point* p5 = new Point(pointList.size(), xP1P3, vP1P3, uP1P3, true);
		pointList.push_back(p5);

		/* find the face neighbours and remesh it */
		Tetrahedron* nf123 = faceNeighour(p1, p2, p3);
		if (nf123 != NULL)
			nf123->remeshByFaceWithTwoNewPoints(p1, p2, p3, p4, p5);

		/* find the edge neighbours and remesh them */
		std::vector<Tetrahedron*> en12, en13;
		Tetrahedron* nf012 = faceNeighour(p0, p1, p2);
		Tetrahedron* nf013 = faceNeighour(p0, p1, p3);

		edgeNeighour(p1, p2, nf012, nf123, en12);
		edgeNeighour(p1, p3, nf013, nf123, en13);

		for (std::vector<Tetrahedron*>::iterator iter = en12.begin(); iter != en12.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p1, p2, p4);
		for (std::vector<Tetrahedron*>::iterator iter = en13.begin(); iter != en13.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p1, p3, p5);

		/* remesh self */
		if (isPositiveRemesh(p1, p2, nvector))
			this->selfRemeshByFaceWithTwoNewPoints_Positive(p1, p2, p3, p4, p5, replica);
		else
			this->selfRemeshByFaceWithTwoNewPoints_Negative(p1, p2, p3, p4, p5, replica);
	}

	else if (state == 1)	// 1 new point between P2 and P3
	{
		/*DEBUG ONLY
		ratioP2P3 = 0.5;
		END DEBUG*/

		/* create the 1 new point */
		double *x2 = p2->getX(), *v2 = p2->getV(), *u2 = p2->getU();
		double *x3 = p3->getX(), *v3 = p3->getV(), *u3 = p3->getU();

		double xP2P3[3], vP2P3[3], uP2P3[3];
		for (int i = 0; i < 3; i++)
		{
			xP2P3[i] = x2[i] + ratioP2P3 * (x3[i] - x2[i]);
			vP2P3[i] = v2[i] + ratioP2P3 * (v3[i] - v2[i]);
			uP2P3[i] = u2[i] + ratioP2P3 * (u3[i] - u2[i]);
		}

		Point* p4 = new Point(pointList.size(), xP2P3, vP2P3, uP2P3, true);
		pointList.push_back(p4);

		/* find the face neighbours and remesh it */
		Tetrahedron* nf123 = faceNeighour(p1, p2, p3);
		if (nf123 != NULL)
			nf123->remeshByFaceWithOneNewPoint(p1, p2, p3, p4);

		/* find the edge neighbours and remesh them */
		std::vector<Tetrahedron*> en23;
		Tetrahedron* nf023 = faceNeighour(p0, p2, p3);

		edgeNeighour(p2, p3, nf023, nf123, en23);

		for (std::vector<Tetrahedron*>::iterator iter = en23.begin(); iter != en23.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p2, p3, p4);

		/* remesh self */
		if (isPositiveRemesh(p2, p3, nvector))
			this->selfRemeshByFaceWithOneNewPoint_Positive(p1, p2, p3, p4, replica);
		else
			this->selfRemeshByFaceWithOneNewPoint_Negative(p1, p2, p3, p4, replica);
	}
	else
	{
		// Must not reach here
	}


}

void Tetrahedron::remesh2(Point* orginal, Matrix<double, 3, 1>& nvector, vector<Point*>& pointList)
{
	/* assume that the given point is P0, the rest is P1, P2 and P3 */
	Point *p0 = orginal, *p1, *p2, *p3;
	
	if (orginal == points[0])
	{
		p1 = points[1];
		p2 = points[2];
		p3 = points[3];
	}
	else if (orginal == points[1])
	{
		p1 = points[0];
		p2 = points[2];
		p3 = points[3];
	}
	else if (orginal == points[2])
	{
		p1 = points[1];
		p2 = points[0];
		p3 = points[3];
	}
	else if (orginal == points[3])
	{
		p1 = points[1];
		p2 = points[2];
		p3 = points[0];
	}
	else
	{
		// must not reach here, it is a fetal error
		util::log("FETAL ERROR IN Tetrahedron::Remesh");
	}
	

	/* calculate the intersected points for P1P2 and P1P3

	NOTE:	a. 2 new points different from P1 P2 and P3
			b. 1 new point, and another is same to P1 P2 or P3
			c. no new point :
				c1. P1P2
				c2. P1P3
				c3. P2P3
	*/

	double ratioP1P2, ratioP1P3, ratioP2P3;
	
	int resP1P2 = util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
	int resP1P3 = util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
	int resP2P3 = util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);

	int state = -1;		// 0 - a, 1 - b, 2 - c

	/* 2 New Points, assert that p4:p1-p2, p5:p1-p3 */
	if (resP1P2 + resP1P3 + resP2P3 == 2)
	{
		state = 0;

		// swap the p1 p2 p3 to keep the assertion
		if (resP1P2 == 0)	//1<->3
		{
			Point* tmp = p1;
			p1 = p3;
			p3 = tmp;

			// update ratio
			util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
			util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
			util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
		}
		else if (resP1P3 == 0)	// 1<->2
		{			
			Point* tmp = p1;
			p1 = p2;
			p2 = tmp;

			// update ratio
			util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
			util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
			util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
		}
		else
		{
			// order is correct
		}
	}
	/* 1 New Points, assert that p4:p2-p3 */
	else if ((resP1P2 == 3 && resP1P3 == 3 && resP2P3 == 1) ||
			 (resP1P2 == 3 && resP1P3 == 0 && resP2P3 == 1) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 1))		// 3,3,1 == 3,0,1 == 0,3,1
	{
		state = 1;
		// order is correct
	}
	else if ((resP1P2 == 1 && resP1P3 == 10 && resP2P3 == 10) ||
			 (resP1P2 == 1 && resP1P3 == 10 && resP2P3 == 0) ||
			 (resP1P2 == 1 && resP1P3 == 0 && resP2P3 == 10))		// 1,10,10 == 1,0,10 == 1,10,0
	{
		state = 1;
		//1<->3
		Point* tmp = p1;
		p1 = p3;
		p3 = tmp;

		// update ratio
		util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
		util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
		util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
	}
	else if ((resP1P2 == 10 && resP1P3 == 1 && resP2P3 == 3) ||
			 (resP1P2 == 10 && resP1P3 == 1 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 1 && resP2P3 == 3))		// 10,1,3 == 10,1,0 == 0,1,3
	{
		state = 1;
		// 1<->2
		Point* tmp = p1;
		p1 = p2;
		p2 = tmp;

		// update ratio
		util::intersect_line_plane(p1->getX(), p2->getX(), nvector, orginal->getX(), ratioP1P2);
		util::intersect_line_plane(p1->getX(), p3->getX(), nvector, orginal->getX(), ratioP1P3);
		util::intersect_line_plane(p2->getX(), p3->getX(), nvector, orginal->getX(), ratioP2P3);
	}
	else if (resP1P2 == 0 && resP1P3 == 0 && resP2P3 == 0)
	{
		// do not intersect with this tetrahedron
	}
	else if ((resP1P2 == 3 && resP1P3 == 3 && resP2P3 == 0) ||
			 (resP1P2 == 10 && resP1P3 == 0 && resP2P3 == 3) ||
			 (resP1P2 == 1 && resP1P3 == 0 && resP2P3 == 3) ||
			 (resP1P2 == 3 && resP1P3 == 0 && resP2P3 == 10) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 1) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 10) ||
			 (resP1P2 == 10 && resP1P3 == 0 && resP2P3 == 1) ||
			 (resP1P2 == 3 && resP1P3 == 1 && resP2P3 == 0) ||
			 (resP1P2 == 10 && resP1P3 == 10 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 3) ||
			 (resP1P2 == 0 && resP1P3 == 1 && resP2P3 == 10) ||
			 (resP1P2 == 1 && resP1P3 == 3 && resP2P3 == 0) ||
			 (resP1P2 == 0 && resP1P3 == 10 && resP2P3 == 3) ||
			 (resP1P2 == 0 && resP1P3 == 3 && resP2P3 == 10) ||
			 (resP1P2 == 3 && resP1P3 == 10 && resP2P3 == 0) ||
			 (resP1P2 == 3 && resP1P3 == 0 && resP2P3 == 3))
	{
		// ignore
	}
	else
	{
		util::log("CASE NOT SOLVED: Tetrahedron::remesh");
	}

	if (state == 0)	/* 2 New Points, assert that p4:p1-p2, p5:p1-p3 */
	{
		/*DEBUG ONLY
		ratioP1P2 = 0.5;
		ratioP1P3 = 0.5;
		END DEBUG*/

		/* create the 2 new points */
		double *x1 = p1->getX(), *v1 = p1->getV(), *u1 = p1->getU();
		double *x2 = p2->getX(), *v2 = p2->getV(), *u2 = p2->getU();
		double *x3 = p3->getX(), *v3 = p3->getV(), *u3 = p3->getU();

		// np1 : between P1 and P2, np1 = p1 + t * (p2 - p1)
		// np2 : between P1 and P3, np2 = p1 + t * (p3 - p1)
		double xP1P2[3], vP1P2[3], uP1P2[3];
		double xP1P3[3], vP1P3[3], uP1P3[3];

		for (int i = 0; i < 3; i++)
		{
			xP1P2[i] = x1[i] + ratioP1P2 * (x2[i] - x1[i]);
			vP1P2[i] = v1[i] + ratioP1P2 * (v2[i] - v1[i]);
			uP1P2[i] = u1[i] + ratioP1P2 * (u2[i] - u1[i]);

			xP1P3[i] = x1[i] + ratioP1P3 * (x3[i] - x1[i]);
			vP1P3[i] = v1[i] + ratioP1P3 * (v3[i] - v1[i]);
			uP1P3[i] = u1[i] + ratioP1P3 * (u3[i] - u1[i]);
		}

		Point* p4 = new Point(pointList.size(), xP1P2, vP1P2, uP1P2, true);
		pointList.push_back(p4);
		Point* p5 = new Point(pointList.size(), xP1P3, vP1P3, uP1P3, true);
		pointList.push_back(p5);

		/* find the face neighbours and remesh it */
		Tetrahedron* nf123 = faceNeighour(p1, p2, p3);
		if (nf123 != NULL)
			nf123->remeshByFaceWithTwoNewPoints(p1, p2, p3, p4, p5);

		/* find the edge neighbours and remesh them */
		std::vector<Tetrahedron*> en12, en13;
		Tetrahedron* nf012 = faceNeighour(p0, p1, p2);
		Tetrahedron* nf013 = faceNeighour(p0, p1, p3);

		edgeNeighour(p1, p2, nf012, nf123, en12);
		edgeNeighour(p1, p3, nf013, nf123, en13);

		for (std::vector<Tetrahedron*>::iterator iter = en12.begin(); iter != en12.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p1, p2, p4);
		for (std::vector<Tetrahedron*>::iterator iter = en13.begin(); iter != en13.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p1, p3, p5);

		/* remesh self */
		this->remeshByFaceWithTwoNewPoints(p1, p2, p3, p4, p5);
	}

	else if (state == 1)	// 1 new point between P2 and P3
	{
		/*DEBUG ONLY
		ratioP2P3 = 0.5;
		END DEBUG*/

		/* create the 1 new point */
		double *x2 = p2->getX(), *v2 = p2->getV(), *u2 = p2->getU();
		double *x3 = p3->getX(), *v3 = p3->getV(), *u3 = p3->getU();

		double xP2P3[3], vP2P3[3], uP2P3[3];
		for (int i = 0; i < 3; i++)
		{
			xP2P3[i] = x2[i] + ratioP2P3 * (x3[i] - x2[i]);
			vP2P3[i] = v2[i] + ratioP2P3 * (v3[i] - v2[i]);
			uP2P3[i] = u2[i] + ratioP2P3 * (u3[i] - u2[i]);
		}

		Point* p4 = new Point(pointList.size(), xP2P3, vP2P3, uP2P3, true);
		pointList.push_back(p4);

		/* find the face neighbours and remesh it */
		Tetrahedron* nf123 = faceNeighour(p1, p2, p3);
		if (nf123 != NULL)
			nf123->remeshByFaceWithOneNewPoint(p1, p2, p3, p4);

		/* find the edge neighbours and remesh them */
		std::vector<Tetrahedron*> en23;
		Tetrahedron* nf023 = faceNeighour(p0, p2, p3);

		edgeNeighour(p2, p3, nf023, nf123, en23);

		for (std::vector<Tetrahedron*>::iterator iter = en23.begin(); iter != en23.end(); ++iter)
			(*iter)->remeshByEdgeWithOneNewPoint(p2, p3, p4);

		/* remesh self */
		this->remeshByFaceWithOneNewPoint(p1, p2, p3, p4);
	}
	else
	{
		// Must not reach here
	}


}

void Tetrahedron::remeshByFaceWithTwoNewPoints(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5)
{
	std::vector<Tetrahedron*>* tetList = volume->getTetrahedra();
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 3 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 and new point 4 5
			A - 1 0 4 5
			B - 4 0 2 5
			C - 5 0 2 3 (Keep the ID)

			0: C +A +B
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(p0);
	vpA.push_back(p4);
	vpA.push_back(p5);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p4);
	vpB.push_back(p0);
	vpB.push_back(p2);
	vpB.push_back(p5);

	// C
	std::vector<Point*> vpC;
	vpC.push_back(p5);
	vpC.push_back(p0);
	vpC.push_back(p2);
	vpC.push_back(p3);

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);
	Tetrahedron* tetB = new Tetrahedron(tetList->size(), volume, vpB);
	tetList->push_back(tetB);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetC = new Tetrahedron(id, volume, vpC);
	tetList->at(id) = tetC;

	/* Inverse Index */
		/*
			For orginal Point 0 1 2 3 and new point 4 5
			A - 1 0 4 5
			B - 4 0 2 5
			C - 5 0 2 3 (Keep the ID)

			0: C +A +B
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
		*/

	//PO
	p0->modifyReverseIndex(tetC, 1, id);
	p0->addReverseIndex(tetA, 1);
	p0->addReverseIndex(tetB, 1);

	//P1
	p1->removeReverseIndex(id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetC, 2, id);
	p2->addReverseIndex(tetB, 2);

	//P3
	p3->modifyReverseIndex(tetC, 3, id);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 0);

	//P5
	p5->addReverseIndex(tetA, 3);
	p5->addReverseIndex(tetB, 3);
	p5->addReverseIndex(tetC, 0);
	
	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::selfRemeshByFaceWithTwoNewPoints_Positive(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* pR)
{
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 3 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 R and new point 4 5
			A - 1 0 4 5
			B - 4 R 2 5
			C - 5 R 2 3 (Keep the ID)

			0: -C +A
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
			R: +B +C
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(p0);
	vpA.push_back(p4);
	vpA.push_back(p5);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p4);
	vpB.push_back(pR);
	vpB.push_back(p2);
	vpB.push_back(p5);

	// C
	std::vector<Point*> vpC;
	vpC.push_back(p5);
	vpC.push_back(pR);
	vpC.push_back(p2);
	vpC.push_back(p3);

	vector<Tetrahedron*>* tetList = volume->getTetrahedra();

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);
	Tetrahedron* tetB = new Tetrahedron(tetList->size(), volume, vpB);
	tetList->push_back(tetB);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetC = new Tetrahedron(id, volume, vpC);
	tetList->at(id) = tetC;

	/* Inverse Index */
		/*
			For orginal Point 0 1 2 3 R and new point 4 5
			A - 1 0 4 5
			B - 4 R 2 5
			C - 5 R 2 3 (Keep the ID)

			0: -C +A
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
			R: +B +C
		*/

	//PO
	p0->removeReverseIndex(id);
	p0->addReverseIndex(tetA, 1);

	//P1
	p1->removeReverseIndex(id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetC, 2, id);
	p2->addReverseIndex(tetB, 2);

	//P3
	p3->modifyReverseIndex(tetC, 3, id);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 0);

	//P5
	p5->addReverseIndex(tetA, 3);
	p5->addReverseIndex(tetB, 3);
	p5->addReverseIndex(tetC, 0);

	//PR
	pR->addReverseIndex(tetB, 1);
	pR->addReverseIndex(tetC, 1);
	
	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::selfRemeshByFaceWithTwoNewPoints_Negative(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* pR)
{
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 3 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 R and new point 4 5
			A - 1 R 4 5
			B - 4 0 2 5
			C - 5 0 2 3 (Keep the ID)

			0: C +B
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
			R: +A
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(pR);
	vpA.push_back(p4);
	vpA.push_back(p5);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p4);
	vpB.push_back(p0);
	vpB.push_back(p2);
	vpB.push_back(p5);

	// C
	std::vector<Point*> vpC;
	vpC.push_back(p5);
	vpC.push_back(p0);
	vpC.push_back(p2);
	vpC.push_back(p3);

	vector<Tetrahedron*>* tetList = volume->getTetrahedra();

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);
	Tetrahedron* tetB = new Tetrahedron(tetList->size(), volume, vpB);
	tetList->push_back(tetB);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetC = new Tetrahedron(id, volume, vpC);
	tetList->at(id) = tetC;

	/* Inverse Index */
		/*
			For orginal Point 0 1 2 3 R and new point 4 5
			A - 1 R 4 5
			B - 4 0 2 5
			C - 5 0 2 3 (Keep the ID)

			0: C +B
			1: -C +A
			2: C +B
			3: C
			4: +A +B
			5: +A +B +C
			R: +A
		*/

	//PO
	p0->modifyReverseIndex(tetC, 1, id);
	p0->addReverseIndex(tetB, 1);

	//P1
	p1->removeReverseIndex(id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetC, 2, id);
	p2->addReverseIndex(tetB, 2);

	//P3
	p3->modifyReverseIndex(tetC, 3, id);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 0);

	//P5
	p5->addReverseIndex(tetA, 3);
	p5->addReverseIndex(tetB, 3);
	p5->addReverseIndex(tetC, 0);

	//PR
	pR->addReverseIndex(tetA, 1);
	
	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::remeshByFaceWithOneNewPoint(Point* p1, Point* p2, Point* p3, Point* p4)
{
	std::vector<Tetrahedron*>* tetList = volume->getTetrahedra();
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 2 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 0 4 3
			B - 0 2 4 1 (Keep the ID)

			0 - B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(p0);
	vpA.push_back(p4);
	vpA.push_back(p3);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p0);
	vpB.push_back(p2);
	vpB.push_back(p4);
	vpB.push_back(p1);

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetB = new Tetrahedron(id, volume, vpB);
	tetList->at(id) = tetB;

	/* Reverse Index */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 0 4 3
			B - 0 2 4 1 (Keep the ID)

			0 - B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
		*/

	//P0
	p0->modifyReverseIndex(tetB, 0, id);
	p0->addReverseIndex(tetA, 1);

	//P1
	p1->modifyReverseIndex(tetB, 3, id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetB, 1, id);

	//P3
	p3->removeReverseIndex(id);
	p3->addReverseIndex(tetA, 3);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 2);

	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::selfRemeshByFaceWithOneNewPoint_Positive(Point* p1, Point* p2, Point* p3, Point* p4, Point* pR)
{
	std::vector<Tetrahedron*>* tetList = volume->getTetrahedra();
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 2 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 R 4 3
			B - 0 2 4 1 (Keep the ID)

			0 - B
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
			R - +A
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(pR);
	vpA.push_back(p4);
	vpA.push_back(p3);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p0);
	vpB.push_back(p2);
	vpB.push_back(p4);
	vpB.push_back(p1);

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetB = new Tetrahedron(id, volume, vpB);
	tetList->at(id) = tetB;

	/* Reverse Index */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 R 4 3
			B - 0 2 4 1 (Keep the ID)

			0 - B
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
			R - +A
		*/

	//P0
	p0->modifyReverseIndex(tetB, 0, id);

	//P1
	p1->modifyReverseIndex(tetB, 3, id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetB, 1, id);

	//P3
	p3->removeReverseIndex(id);
	p3->addReverseIndex(tetA, 3);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 2);

	//PR
	pR->addReverseIndex(tetA, 1);

	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::selfRemeshByFaceWithOneNewPoint_Negative(Point* p1, Point* p2, Point* p3, Point* p4, Point* pR)
{
	std::vector<Tetrahedron*>* tetList = volume->getTetrahedra();
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 2 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 0 4 3
			B - R 2 4 1 (Keep the ID)

			0 - -B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
			R - +B
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p1);
	vpA.push_back(p0);
	vpA.push_back(p4);
	vpA.push_back(p3);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(pR);
	vpB.push_back(p2);
	vpB.push_back(p4);
	vpB.push_back(p1);

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetB = new Tetrahedron(id, volume, vpB);
	tetList->at(id) = tetB;

	/* Reverse Index */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p1 and p2
			A - 1 0 4 3
			B - R 2 4 1 (Keep the ID)

			0 - -B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
			R - +B
		*/

	//P0
	p0->removeReverseIndex(id);
	p0->addReverseIndex(tetA, 1);

	//P1
	p1->modifyReverseIndex(tetB, 3, id);
	p1->addReverseIndex(tetA, 0);

	//P2
	p2->modifyReverseIndex(tetB, 1, id);

	//P3
	p3->removeReverseIndex(id);
	p3->addReverseIndex(tetA, 3);

	//P4
	p4->addReverseIndex(tetA, 2);
	p4->addReverseIndex(tetB, 2);

	//PR
	pR->addReverseIndex(tetB, 0);

	/* Delete old tetrahedron */
	//delete toFree;
}

void Tetrahedron::remeshByEdgeWithOneNewPoint(Point* p2, Point* p3, Point* p4)
{
	std::vector<Tetrahedron*>* tetList = volume->getTetrahedra();
	// find p0 and p1
	Point* p1 = getFacePoint(p2, p3, p3);
	Point* p0 = getFacePoint(p1, p2, p3);

	/* create new 2 tetrahedrons for current tetrahedron */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p2 and p3
			A - 0 1 3 4
			B - 0 1 2 4 (Keep the ID)

			0 - B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
		*/

	// A
	std::vector<Point*> vpA;
	vpA.push_back(p0);
	vpA.push_back(p1);
	vpA.push_back(p3);
	vpA.push_back(p4);

	// B
	std::vector<Point*> vpB;
	vpB.push_back(p0);
	vpB.push_back(p1);
	vpB.push_back(p2);
	vpB.push_back(p4);

	Tetrahedron* tetA = new Tetrahedron(tetList->size(), volume, vpA);
	tetList->push_back(tetA);

	Tetrahedron* toFree = tetList->at(id);
	Tetrahedron* tetB = new Tetrahedron(id, volume, vpB);
	tetList->at(id) = tetB;

	/* Reverse Index */
		/*
			For orginal Point 0 1 2 3 and new point 4

			p4 is between p2 and p3
			A - 0 1 3 4
			B - 0 1 2 4 (Keep the ID)

			0 - B +A
			1 - B +A
			2 - B
			3 - -B +A
			4 - +A +B
		*/

	//P0
	p0->modifyReverseIndex(tetB, 0, id);
	p0->addReverseIndex(tetA, 0);

	//P1
	p1->modifyReverseIndex(tetB, 1, id);
	p1->addReverseIndex(tetA, 1);

	//P2
	p2->modifyReverseIndex(tetB, 2, id);

	//P3
	p3->removeReverseIndex(id);
	p3->addReverseIndex(tetA, 2);

	//P4
	p4->addReverseIndex(tetA, 3);
	p4->addReverseIndex(tetB, 3);

	/* Delete old tetrahedron */
	//delete toFree;
}

Tetrahedron* Tetrahedron::faceNeighour(Point* a, Point* b, Point* c)
{
	std::vector<struct IndexTetraPoint>* va = a->getIndexTetra();
	std::vector<struct IndexTetraPoint>* vb = b->getIndexTetra();
	std::vector<struct IndexTetraPoint>* vc = c->getIndexTetra();

	std::vector<Tetrahedron*> commonAB;
	
	// find those both in va and vb
	for (int i = 0; i < (int)va->size(); i++)
	{
		Tetrahedron* base = va->at(i).tet;

		if (base == this)
			continue;

		for (int j = 0; j < (int)vb->size(); j++)
		{
			Tetrahedron* comp = vb->at(j).tet;
			if (base == comp)
			{
				commonAB.push_back(base);
				break;
			}
		}
	}

	std::vector<Tetrahedron*> common;

	// find those both in vc and commonAB, normally only one could be found
	for (int i = 0; i < (int)commonAB.size(); i++)
	{
		Tetrahedron* base = commonAB.at(i);

		for (int j = 0; j < (int)vc->size(); j++)
		{
			Tetrahedron* comp = vc->at(j).tet;
			if (base == comp)
			{
				common.push_back(base);
				break;
			}
		}
	}

	if (common.size() > 1)
	{
		// ERROR
		util::log("FETAL ERROR in Tetrahedron::faceNeighour");
		return NULL;
	}
	else if (common.size() == 0)
	{
		return NULL;
	}
	else
		return common[0];
}

void Tetrahedron::edgeNeighour(Point* a, Point* b, Tetrahedron* faceNeighbourA, Tetrahedron* faceNeighbourB, std::vector<Tetrahedron*>& commonAB)
{
	std::vector<struct IndexTetraPoint>* va = a->getIndexTetra();
	std::vector<struct IndexTetraPoint>* vb = b->getIndexTetra();
	
	// find those both in va and vb
	for (int i = 0; i < (int)va->size(); i++)
	{
		Tetrahedron* base = va->at(i).tet;

		if (base == faceNeighbourA || base == faceNeighbourB || base == this)
			continue;

		for (int j = 0; j < (int)vb->size(); j++)
		{
			Tetrahedron* comp = vb->at(j).tet;
			if (base == comp)
			{
				commonAB.push_back(base);
				break;
			}
		}
	}
}

Point* Tetrahedron::getFacePoint(Point* a, Point* b, Point* c)
{
	for (int i = 0; i < (int)points.size(); i++)
	{
		Point* base = points[i];
		if (base != a && base != b && base != c)
			return base;
	}

	util::log("FETAL ERROR: Could not reach here: Tetrahedron::getFacePoint");
	return NULL;
}

bool Tetrahedron::isPositiveRemesh(Point* p1, Point* p2, Matrix<double, 3, 1>& nvector)
{
	double* x1 = p1->getX();
	double* x2 = p2->getX();
	double ret = (x1[0] - x2[0]) * nvector(0,0) +
				(x1[1] - x2[1]) * nvector(1,0) +
				(x1[2] - x2[2]) * nvector(2,0);
	
	if (ret > 0)
		return true;
	return false;
}