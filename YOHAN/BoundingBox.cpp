#include "BoundingBox.h"

BoundingBox::BoundingBox(BoundingBox* parent, vector<Point*> points)
{
	this->parent = parent;
	this->child1 = NULL;
	this->child2 = NULL;
	x1=y1=z1=x2=y2=z2=0;
	this->points = points;

	if (points.size() > 4)
	{
		recalculateBoundingBox();
		createChildren();
	}
	else
	{
		findTetrahedra();
	}
}

BoundingBox::BoundingBox(BoundingBox* parent, double x1, double y1, double z1, double x2, double y2, double z2, bool fx1, bool fy1, bool fz1, bool fx2, bool fy2, bool fz2)
{
	this->parent = parent;
	this->child1 = NULL;
	this->child2 = NULL;
	this->points = *(parent->getPoints());
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	this->z1 = z1;
	this->z2 = z2;

	recalculatePointsList();
	if (points.size() > 4)
	{
		recalculateBoundingBox(fx1, fy1, fz1, fx2, fy2, fz2);
		createChildren();
	}
	else
	{
		findTetrahedra();
	}
}

BoundingBox::~BoundingBox(void)
{
}


vector<Point*>* BoundingBox::getPoints()
{
	return &points;
}

void BoundingBox::recalculateBoundingBox(bool fx1, bool fy1, bool fz1, bool fx2, bool fy2, bool fz2)
{
	int size = points.size();

	if (size == 0)
		return;

	double* x = points[0]->getX();
	x1 = x2 = x[0];
	y1 = y2 = x[1];
	z1 = z2 = x[2];
	for (int i=1; i < size; i++)
	{
		x = points[i]->getX();
		if (!fx1 && x[0] < x1) x1 = x[0];
		else if (!fx2 && x[0] > x2) x2 = x[0];
		if (!fy1 && x[1] < y1) y1 = x[1];
		else if (!fy2 && x[1] > y2) y2 = x[1];
		if (!fz1 && x[2] < z1) z1 = x[2];
		else if (!fz2 && x[2] > z2) z2 = x[2];
	}

	delete[] x;
}

void BoundingBox::recalculatePointsList()
{
	int size = points.size();

	if (size == 0 || x1 == x2 || y1 == y2 || z1 == z2)
		return;

	double* x;
	vector<Point*> tmp_points;

	for (int i=0; i < size; i++)
	{
		x = points[i]->getX();
		if (x[0] >= x1 && x[0] <= x2 &&
			x[1] >= y1 && x[1] <= y2 &&
			x[2] >= z1 && x[2] <= z2)
		{
			tmp_points.push_back( points[i] );
		}
	}

	points = tmp_points;

	delete[] x;
}

void BoundingBox::createChildren()
{
	int size = points.size();

	if (size == 0 || x1 == x2 || y1 == y2 || z1 == z2)
		return;

	// compute sizes
	double sx = abs(x2-x1);
	double sy = abs(y2-y1);
	double sz = abs(z2-z1);

	if (sx > sy && sx > sz) // sx is biggest
	{
		child1 = new BoundingBox(this, x1, y1, z1, x2 - sx/2, y2, z2, false, false, false, true, false, false);
		child2 = new BoundingBox(this, x1 + sx/2, y1, z1, x2, y2, z2, true, false, false, false, false, false);
	}
	else if (sy > sz) // sy is biggest
	{
		child1 = new BoundingBox(this, x1, y1, z1, x2, y2 - sy/2, z2, false, false, false, false, true, false);
		child2 = new BoundingBox(this, x1, y1 + sy/2, z1, x2, y2, z2, false, true, false, false, false, false);
	}
	else // sz is biggest
	{
		child1 = new BoundingBox(this, x1, y1, z1, x2, y2, z2 - sz/2, false, false, false, false, false, true);
		child2 = new BoundingBox(this, x1, y1, z1 + sz/2, x2, y2, z2, false, false, true, false, false, false);
	}
}

bool BoundingBox::collision(BoundingBox* other)
{
	return ((this->x1 >= other->x1 && this->x1 <= other->x2
		 ||  this->x2 >= other->x1 && this->x2 <= other->x2
		 ||  this->x2 <= other->x1 && this->x2 >= other->x2
		 ||  this->x1 <= other->x1 && this->x1 >= other->x2)
		 && (this->y1 >= other->y1 && this->y1 <= other->y2
		 ||  this->y2 >= other->y1 && this->y2 <= other->y2
		 ||  this->y2 <= other->y1 && this->y2 >= other->y2
		 ||  this->y1 <= other->y1 && this->y1 >= other->y2)
		 && (this->z1 >= other->z1 && this->z1 <= other->z2
		 ||  this->z2 >= other->z1 && this->z2 <= other->z2
		 ||  this->z2 <= other->z1 && this->z2 >= other->z2
		 ||  this->z1 <= other->z1 && this->z1 >= other->z2));
}

void BoundingBox::findTetrahedra()
{
	int size = points.size();

	if (size == 0 || x1 == x2 || y1 == y2 || z1 == z2)
		return;

	vector<Tetrahedron*> tmp_tetrahedra;

	for (int i=0; i < size; i++)
	{
		tmp_tetrahedra = *(points[i]->getTetrahedra());
		for (int j=0; j < tmp_tetrahedr.size(); j++)
		tetrahedra.push_back( tmp_tetrahedr[j] );
	}
}