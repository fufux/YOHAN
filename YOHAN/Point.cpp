
#include "Point.h"
#include "Tetrahedron.h"

Point::Point(int id, double* x, double* v, double* u, bool is_surface)
{
	this->id = id;
	for (int i=0; i<3; i++) {
		this->x[i] = x[i];
		this->u[i] = u[i];
		this->v[i] = v[i];
	}
	this->is_surface = is_surface;
}

Point::~Point(void)
{
}

int Point::getID()
{
	return id;
}

double* Point::getX()
{
	return (double*)x;
}

double* Point::getV()
{
	return (double*)v;
}

double* Point::getU()
{
	return (double*)u;
}

bool Point::isSurface()
{
	return is_surface;
}

void Point::setIsSurface(bool is_surface)
{
	this->is_surface = is_surface;
}
