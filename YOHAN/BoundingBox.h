#pragma once

#include "Point.h"
#include "Tetrahedron.h"

class BoundingBox
{
public:
	BoundingBox(BoundingBox* parent, vector<Point*> points);
	BoundingBox(BoundingBox* parent, double x1, double y1, double z1, double x2, double y2, double z2, bool fx1, bool fy1, bool fz1, bool fx2, bool fy2, bool fz2);
	~BoundingBox(void);

	// returns a pointer to the list of internal points
	vector<Point*>* getPoints();

	// recalculate x1,y1,z1,x2,y2 and z2 using the list of internal points.
	// you can force it to not change x1,y1,z1,x2,y2 and/or z2.
	void recalculateBoundingBox(bool fx1=false, bool fy1=false, bool fz1=false, bool fx2=false, bool fy2=false, bool fz2=false);
	
	// recalculate internal points using x1,y1,z1,x2,y2 and z2.
	void recalculatePointsList();

	// returns true is this bounding box and the other one are colliding. Else returns false.
	bool collision(BoundingBox* other);

private:
	double x1,y1,z1;
	double x2,y2,z2;

	// parent bounding box
	BoundingBox* parent;

	// left and right children in the tree
	BoundingBox *child1, *child2;

	// if there are not enought internal points, we stop creating children and find tetrahedra instead.
	vector<Tetrahedron*> tetrahedra;

	// internal points of this bounding box
	vector<Point*> points;

	void createChildren();
	void findTetrahedra();
};
