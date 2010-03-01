#pragma once

#include "stdafx.h"
#include <Eigen/Eigen>
#include "Point.h"
USING_PART_OF_NAMESPACE_EIGEN

class Face
{
private:
	vector<Vector3d*>* vertices;
	int owner;

public:
	Face(void);
	// Vertices must be given clockwise
	Face(vector<Vector3d*>* _vertices, int _owner);
	//
	Face(vector<Point*>* pts, int i1, int i2, int i3, Vector3d*** m, int _owner);
	~Face(void);

	// Returns the vertices
	vector<Vector3d*>* getVertices();

	// Returns the tetrahedron index from which the facet comes from 
	int getOwner();
	
	// Returns the number of vertices in the facet
	int size();

	// Table kind accessor
	Vector3d* operator[] (int);
};
