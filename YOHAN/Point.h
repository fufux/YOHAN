#pragma once

#include "stdafx.h"

class Tetrahedron;

/* Added by Ning */
struct Surface;

struct IndexTetraPoint
{
	Tetrahedron* tet;
	int indexOfPoint;
};

struct IndexSurfacePoint
{
	Surface* surface;
	int indexOfPoint;
};

/* END -- Added by Ning */


class Point
{
public:
	Point(int id, double* x, double* v, double* u, bool is_surface=false);
	~Point(void);

	int getID();
	double* getX();
	double* getV();
	double* getU();
	bool isSurface();
	void setIsSurface(bool is_surface);
	double getMass();

	/* Modified by Ning, for fracture */
	//vector<Tetrahedron*>* getTetrahedra();
	vector<struct IndexTetraPoint>* getIndexTetra();	

	vector<struct IndexSurfacePoint>* getIndexSurface();

	void addReverseIndex(Tetrahedron* tetra, int iop);

	void modifyReverseIndex(Tetrahedron* tetra, int iop, int tetID);

	void removeReverseIndex(int tetID);

private:
	// ID of the point
	int id;

	// position
	double x[3]; // array of size 3
	// velocity
	double v[3]; // array of size 3
	// initial position
	double u[3]; // array of size 3

	// list of tetrahedrons containing this point
	vector<struct IndexTetraPoint> indexTetra;

	// true if this point is on the surface of the volume
	bool is_surface;

	/* Added by Ning, for fracture */

	// the point index correspondant with the surface
	vector<struct IndexSurfacePoint> indexSurface;

	/* END -- Added by Ning */

};
