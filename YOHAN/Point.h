#pragma once

#include "stdafx.h"

class Tetrahedron;


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
	vector<Tetrahedron*> tetrahedrons;

	// true if this point is on the surface of the volume
	bool is_surface;
};
