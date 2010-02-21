#pragma once

#include "stdafx.h"
#include "Point.h"

class Tetrahedron
{
public:
	Tetrahedron(int id, vector<Point*> points);
	~Tetrahedron(void);

	vector<Point*> getPoints();
	double** getBeta();
	double*** getForceJacobian();
	double getMass();

private:
	int id;

	// list of the four points making this tetrahedron
	vector<Point*> points; // array of size 4

	// this is the beta in equation (1) of main article
	double beta[3][3]; // 3x3

	/* "The core of (2), consisting of inner and outer normal products,
	does not change as the mesh deforms and can be cached
	for each element." (see main article)
	*/
	double forceJacobian[16][3][3]; // 4x4x3x3

	// mass of this tetrahedron
	double mass;
};
