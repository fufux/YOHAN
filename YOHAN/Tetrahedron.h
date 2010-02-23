#pragma once

#include "stdafx.h"
#include <Eigen/Eigen>
USING_PART_OF_NAMESPACE_EIGEN
#include "Point.h"

class Volume;

class Tetrahedron
{
public:
	Tetrahedron(int id, Volume* volume, vector<Point*> points);
	~Tetrahedron(void);

	vector<Point*> getPoints();
	Matrix3d getBeta();
	vector<Matrix3d> getCoreJacobian();
	double getMass();

private:
	int id;

	Volume* volume;

	// list of the four points making this tetrahedron
	vector<Point*> points; // array of size 4

	// this is the beta in equation (1) of main article
	Matrix3d beta; // 3x3

	/* "The core of (2), consisting of inner and outer normal products,
	does not change as the mesh deforms and can be cached
	for each element." (see main article)
	*/
	vector<Matrix3d> coreJacobian; // 16x3x3

	// mass of this tetrahedron
	double mass;
};
