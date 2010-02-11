#pragma once

#include "CollisionPoint.h"

/**

A class representing a face of a polyedron

*/
class CollisionFace
{
private:
	std::vector<CollisionPoint>* vertices;
	int owner;

public:
	CollisionFace(void);
	// Constructs a facet, vertices must be given clockwise !!!
	CollisionFace(std::vector<CollisionPoint>*,int);
	// Destructs a facet
	~CollisionFace(void);
	// Returns the number of vertices in the facet
	int size();
	// Returns the tetrahedron index from which the facet comes from 
	int getOwner();
	// Table kind accessor
	CollisionPoint& operator[] (int);
};
