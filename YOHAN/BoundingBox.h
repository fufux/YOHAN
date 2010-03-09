#pragma once

#include "Point.h"
#include "Tetrahedron.h"

class BoundingBox
{
public:
	// This constructor is recursive and will build the entire tree !
	BoundingBox(BoundingBox* parent, vector<Tetrahedron*> tetrahedra, bool is_1);
	~BoundingBox(void);

	// returns a pointer to the list of internal points
	vector<Tetrahedron*>* getTetrahedra();

	// recalculate x1,y1,z1,x2,y2 and z2 using the list of internal tetrahedra.
	// you can force it to not change x1,y1,z1,x2,y2 and/or z2.
	void recalculateBoundingBoxes();

	// returns true is this bounding box and the other one are colliding. Else, returns false.
	bool collision(BoundingBox* other);

	// returns true is this bounding box is overlapping the plan Y=y. Else, returns false.
	bool collision(double y);

	// this will update the list of potentialy collinding tetrahedra
	void getCollidingTetrahedra(BoundingBox* other, vector<vector<Tetrahedron*>> *found);

	// this will update the list of potentialy collinding tetrahedra with plan Y=y
	void getCollidingTetrahedra(double y, std::vector<Tetrahedron*> *found);

	// save this bounding box and his children in a file
	void saveAllToFile(std::string filename);

	bool is1();
	bool is2();
	bool isLeaf();
	double getX1();
	double getY1();
	double getZ1();
	double getX2();
	double getY2();
	double getZ2();
	BoundingBox *getChild1();
	BoundingBox *getChild2();

private:
	double x1,y1,z1;
	double x2,y2,z2;
	bool is_1;

	// parent bounding box
	BoundingBox* parent;

	// left and right children in the tree
	BoundingBox *child1, *child2;

	// if there are not enought internal points, we stop creating children and find tetrahedra instead.
	vector<Tetrahedron*> tetrahedra;

	void saveToFile(ofstream &fp);
};
