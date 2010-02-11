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
	~CollisionPolyedron(void);
	CollisionPolyedron(std::vector<CollisionFace>*, int, int);
	std::vector<CollisionFace>* getFaces();
	std::vector<CollisionFace>* getByOwner(int);
	double calcVolume();
	CollisionPoint* calcCenter();
	std::vector<yohan::base::DATA*>* getByParent(yohan::base::VolumeModel*,int);
	CollisionPoint* calcDir(int);
	std::vector<double>* calcBarCoeff(std::vector<yohan::base::DATA*>*);
	void collisionForces(yohan::base::VolumeModel*,double, double ,double, CollisionPoint*);
};
