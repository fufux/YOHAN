
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

/* Deleted by Ning, for fracture */
/*
vector<Tetrahedron*>* Point::getTetrahedra()
{
	return &tetrahedra;
}
*/

double Point::getMass()
{
	/* Modified by Ning, for fracture */
	/*
	double mass = 0;
	for(int i=0; i<(int)tetrahedra.size(); i++)
		mass += tetrahedra[i]->getMass()/4;
	return mass;
	*/

	double mass = 0;
	for(int i=0; i<(int)indexTetra.size(); i++)
		mass += indexTetra[i].tet->getMass() / 4;
	return mass;
}

/* Added by Ning, for fracture */
vector<struct IndexTetraPoint>* Point::getIndexTetra()
{
	return &indexTetra;
}

vector<struct IndexSurfacePoint>* Point::getIndexSurface()
{
	return &indexSurface;
}