#include "CollisionPoint.h"


CollisionPoint::CollisionPoint(void): x(0), y(0), z(0)
{
}

CollisionPoint::CollisionPoint(double _x, double _y, double _z): x(_x), y(_y), z(_z)
{
}

CollisionPoint::CollisionPoint(std::vector<double>* coord)
{
	x = (*coord)[0];
	y = (*coord)[1];
	z = (*coord)[2];
}

CollisionPoint::CollisionPoint(double* coord)
{
	x = coord[0];
	y = coord[1];
	z = coord[2];
}

CollisionPoint::~CollisionPoint(void)
{

}

double CollisionPoint::getX()
{
	return this->x;
}

double CollisionPoint::getY()
{
	return this->y;
}

double CollisionPoint::getZ()
{
	return this->z;
}

void CollisionPoint::add (CollisionPoint* pt)
{
	this->x += pt->getX();
	this->y += pt->getY();
	this->z += pt->getZ();
}

void CollisionPoint::minus (CollisionPoint* pt)
{
	this->x -= pt->getX();
	this->y -= pt->getY();
	this->z -= pt->getZ();
}

void CollisionPoint::mul(double m)
{
	this->x *= m;
	this->y *= m;
	this->z *= m;
}

double CollisionPoint::volOp(CollisionPoint* q,CollisionPoint* pi1,CollisionPoint* pij,CollisionPoint* pij1)
{
	double x1,x2,x3,y1,y2,y3,z1,z2,z3;
	x1 = pij->getX() - pi1->getX();
	x2 = pij1->getX() - pi1->getX();
	x3 = q->getX() - pi1->getX();
	y1 = pij->getY() - pi1->getY();
	y2 = pij1->getY() - pi1->getY();
	y3 = q->getY() - pi1->getY();
	z1 = pij->getZ() - pi1->getZ();
	z2 = pij1->getZ() - pi1->getZ();
	z3 = q->getZ() - pi1->getZ();
	return (y1*z2 - y2*z1)*x3 + (z1*x2 - z2*x1)*y3 + (x1*y2 - x2*y1)*z3;
}

CollisionPoint* CollisionPoint::vect(CollisionPoint* a1,CollisionPoint* b1,CollisionPoint* a2,CollisionPoint* b2)
{
	CollisionPoint* pt;
	std::vector<double>* resul = new std::vector<double>(3,0);
	double x1,x2,y1,y2,z1,z2;
	x1 = a1->getX() - b1->getX();
	y1 = a1->getY() - b1->getY();
	z1 = a1->getZ() - b1->getZ();
	x2 = a2->getX() - b2->getX();
	y2 = a2->getY() - b2->getY();
	z2 = a2->getZ() - b2->getZ();
	(*resul)[0] = y1*z2 - y2*z1;
	(*resul)[1] = z1*x2 - z2*x1;
	(*resul)[2] = x1*y2 - x2*y1;
	pt = new CollisionPoint(resul);
	return pt;
}

double CollisionPoint::scal(CollisionPoint* p1, CollisionPoint* p2)
{
	return p1->getX()*p2->getX() + p1->getY()*p2->getY() + p1->getZ()*p2->getZ();
}

double CollisionPoint::det3(CollisionPoint* p1, CollisionPoint* p2, CollisionPoint* p3)
{
	return (p1->getX())*(p2->getY())*(p3->getZ()) - (p1->getZ())*(p2->getY())*(p3->getX())
		+ (p2->getX())*(p3->getY())*(p1->getZ()) - (p2->getZ())*(p3->getY())*(p1->getX())
		+ (p3->getX())*(p1->getY())*(p2->getZ()) - (p3->getZ())*(p1->getY())*(p2->getX());
}

double CollisionPoint::det4(CollisionPoint* p1, CollisionPoint* p2, CollisionPoint* p3, CollisionPoint* p4)
{
	return CollisionPoint::det3(p2, p3, p4) - CollisionPoint::det3(p1, p3, p4) 
		+ CollisionPoint::det3(p1, p2, p4) - CollisionPoint::det3(p1, p2, p3);
}

double CollisionPoint::distance(CollisionPoint* c, CollisionPoint* pt)
{
	return sqrt(((c->getX())-(pt->getX()))*((c->getX())-(pt->getX()))
		+((c->getY())-(pt->getY()))*((c->getY())-(pt->getY()))
		+((c->getZ())-(pt->getZ()))*((c->getZ())-(pt->getZ())));
}

double CollisionPoint::norm()
{
	return sqrt((this->getX())*(this->getX())+(this->getY())*(this->getY())+(this->getZ())*(this->getZ()));
}