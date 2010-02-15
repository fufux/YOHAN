#pragma once

#include "CollisionFace.h"


/**

A class representing a polyhedron

*/
class CollisionPolyedron
{
private:
	std::vector<CollisionFace>* faces;
	// index of the two tetrahedrons from which the polyhedron comes from
	int parents[2];
	double volume;
	CollisionPoint center;

public:
	CollisionPolyedron(void);
	// Destructs the polyhedron
	~CollisionPolyedron(void);
	// Constructs a polyhedron with given facets
	CollisionPolyedron(std::vector<CollisionFace>*, int, int);
	// Returns the facets
	std::vector<CollisionFace>* getFaces();
	// Returns facets which comes from designated tetrahedron
	std::vector<CollisionFace>* getByOwner(int);
	// Calculates the volume of the polyhedron
	double calcVolume();
	// Calculates coordinates of the mass center of the polyhedron
	CollisionPoint* calcCenter();
	// Returns vertices of the designated tetrahedron
	std::vector<yohan::base::DATA*>* getByParent(yohan::base::VolumeModel*,int);
	// Calculates the direction of the collision response
	CollisionPoint* calcDir(int);
	// Calculates barycentric coefficients for each point
	std::vector<double>* calcBarCoeff(std::vector<yohan::base::DATA*>*);
	// Calculates the collision response force
	void collisionForces(yohan::base::VolumeModel*,double, double ,double, CollisionPoint*);
};
