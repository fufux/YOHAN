#include "BoundingBox.h"

BoundingBox::BoundingBox(BoundingBox* parent, vector<Tetrahedron*> tetrahedra, bool is_1)
{
	this->parent = parent;
	this->child1 = NULL;
	this->child2 = NULL;
	this->is_1 = is_1;
	x1=y1=z1=x2=y2=z2=0;
	this->tetrahedra = tetrahedra;

	int size = tetrahedra.size();
	

	// in this case we need to split this bounding box in two sub-(bounding boxes).
	if (size > 1)
	{
		double* center = new double[3];
		double* tcenter = new double[3];
		center[0] = center[1] = center[2] = 0;
		
		double* x = tetrahedra[0]->getPoints()[0]->getX();
		x1 = x2 = x[0];
		y1 = y2 = x[1];
		z1 = z2 = x[2];

		// compute center of mass and bounds
		for (int i=0; i < size; i++) // each internal tetrahedron
		{
			for (int j=0; j < 4; j++) // each point of the tetrahedron
			{
				x = tetrahedra[i]->getPoints()[j]->getX();

				// center of mass
				center[0] += x[0];
				center[1] += x[1];
				center[2] += x[2];
				
				// bounds
				if (x[0] < x1) x1 = x[0];
				else if (x[0] > x2) x2 = x[0];
				if (x[1] < y1) y1 = x[1];
				else if (x[1] > y2) y2 = x[1];
				if (x[2] < z1) z1 = x[2];
				else if (x[2] > z2) z2 = x[2];
			}
		}

		center[0] /= size * 4;
		center[1] /= size * 4;
		center[2] /= size * 4;

		// prepare new lists
		vector<Tetrahedron*> tetrahedra1, tetrahedra2;
		double sx = abs(x2-x1);
		double sy = abs(y2-y1);
		double sz = abs(z2-z1);

		// choose on which plan we are going to split the box : XY, XZ or YZ ?
		int xyz;
		if (sx > sy && sx > sz) // sx is biggest : cut on YZ
			xyz = 0;
		else if (sy > sz) // sy is biggest : cut on XZ
			xyz = 1;
		else // sz is biggest : cut on XY
			xyz = 2;

		// compute new lists
		for (int i=0; i < size; i++) // each internal tetrahedron
		{
			tcenter[0] = tcenter[1] = tcenter[2] = 0;
			for (int j=0; j < 4; j++) // each point of the tetrahedron
			{
				x = tetrahedra[i]->getPoints()[j]->getX();
				// center of mass of tetrahedron
				tcenter[0] += x[0];
				tcenter[1] += x[1];
				tcenter[2] += x[2];
			}
			if (tcenter[xyz] < center[xyz]) 
				tetrahedra1.push_back( tetrahedra[i] ); // add tetrahedron to first child
			else
				tetrahedra2.push_back( tetrahedra[i] ); // add tetrahedron to second child
		}

		this->child1 = new BoundingBox(this, tetrahedra1, true);
		this->child2 = new BoundingBox(this, tetrahedra2, false);

		delete[] x;
		delete[] center;
		delete[] tcenter;
	}
	// in this case, just compute bounds of the uniq tetrahedron
	else if (size == 1)
	{
		double* x = tetrahedra[0]->getPoints()[0]->getX();
		x1 = x2 = x[0];
		y1 = y2 = x[1];
		z1 = z2 = x[2];

		for (int j=1; j < 4; j++) // each point of the tetrahedron
		{
			x = tetrahedra[0]->getPoints()[j]->getX();
			
			// bounds
			if (x[0] < x1) x1 = x[0];
			else if (x[0] > x2) x2 = x[0];
			if (x[1] < y1) y1 = x[1];
			else if (x[1] > y2) y2 = x[1];
			if (x[2] < z1) z1 = x[2];
			else if (x[2] > z2) z2 = x[2];
		}

		delete[] x;
	}
}



BoundingBox::~BoundingBox(void)
{
}


vector<Tetrahedron*>* BoundingBox::getTetrahedra()
{
	return &tetrahedra;
}

bool BoundingBox::is1()
{
	return is_1;
}

bool BoundingBox::is2()
{
	return !is_1;
}

bool BoundingBox::isLeaf()
{
	return (this->child1 == NULL && this->child2 == NULL);
}

double BoundingBox::getX1()
{
	return x1;
}
double BoundingBox::getY1()
{
	return y1;
}
double BoundingBox::getZ1()
{
	return z1;
}
double BoundingBox::getX2()
{
	return x2;
}
double BoundingBox::getY2()
{
	return y2;
}
double BoundingBox::getZ2()
{
	return z2;
}

BoundingBox* BoundingBox::getChild1()
{
	return child1;
}
BoundingBox* BoundingBox::getChild2()
{
	return child2;
}

void BoundingBox::recalculateBoundingBoxes()
{
	if (this->child1 && this->child2)
	{
		this->child1->recalculateBoundingBoxes();
		this->child2->recalculateBoundingBoxes();
		
		// recalculate bounds using only children's bounds
		if (child1->getX1() < child2->getX1())
			x1 = child1->getX1();
		else
			x1 = child2->getX1();
		if (child1->getY1() < child2->getY1())
			y1 = child1->getY1();
		else
			y1 = child2->getY1();
		if (child1->getZ1() < child2->getZ1())
			z1 = child1->getZ1();
		else
			z1 = child2->getZ1();

		if (child1->getX2() < child2->getX2())
			x2 = child2->getX2();
		else
			x2 = child1->getX2();
		if (child1->getY2() < child2->getY2())
			y2 = child2->getY2();
		else
			y2 = child1->getY2();
		if (child1->getZ2() < child2->getZ2())
			z2 = child2->getZ2();
		else
			z2 = child1->getZ2();
	}
	else
	{
		double* x = tetrahedra[0]->getPoints()[0]->getX();
		x1 = x2 = x[0];
		y1 = y2 = x[1];
		z1 = z2 = x[2];

		for (int j=1; j < 4; j++) // each point of the tetrahedron
		{
			x = tetrahedra[0]->getPoints()[j]->getX();
			
			// bounds
			if (x[0] < x1) x1 = x[0];
			else if (x[0] > x2) x2 = x[0];
			if (x[1] < y1) y1 = x[1];
			else if (x[1] > y2) y2 = x[1];
			if (x[2] < z1) z1 = x[2];
			else if (x[2] > z2) z2 = x[2];
		}

		delete[] x;
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


bool BoundingBox::collision(double y)
{
	return (this->y1 < y);
}


void BoundingBox::getCollidingTetrahedra(BoundingBox* other, std::vector<Tetrahedron**> *found)
{
	if (collision(other)) // if bounds overlap ( a, b )
	{
		if (this->isLeaf() && other->isLeaf()) // if are leaf nodes ( a, b )
		{
			// insert a pair of tetrahedra colliding
			Tetrahedron** tets = new Tetrahedron*[2];
			tets[0] = tetrahedra[0];
			tets[1] = (*other->getTetrahedra())[0];
			found->push_back( tets );
		}
		else if (this->isLeaf()) // if is a leaf node ( a )
		{
			other->getChild1()->getCollidingTetrahedra(this, found);
			other->getChild2()->getCollidingTetrahedra(this, found);
		}
		else if (other->isLeaf()) // if is a leaf node ( b )
		{
			this->child1->getCollidingTetrahedra(other, found);
			this->child2->getCollidingTetrahedra(other, found);
		}
		else
		{
			this->child1->getCollidingTetrahedra(other->getChild1(), found);
			this->child1->getCollidingTetrahedra(other->getChild2(), found);
			this->child2->getCollidingTetrahedra(other->getChild1(), found);
			this->child2->getCollidingTetrahedra(other->getChild2(), found);
		}
	}
}



void BoundingBox::getCollidingTetrahedra(double y, std::vector<Tetrahedron*> *found)
{
	if (collision(y)) // if bounds overlap Y=y
	{
		if (this->isLeaf())
		{
			// insert a pair of tetrahedra colliding
			found->push_back( tetrahedra[0] );
		}
		else
		{
			this->child1->getCollidingTetrahedra(y, found);
			this->child2->getCollidingTetrahedra(y, found);
		}
	}
}


void BoundingBox::saveToFile(ofstream &fp)
{
	// write me
	fp.write ((char*)&x1, sizeof(double));
	fp.write ((char*)&y1, sizeof(double));
	fp.write ((char*)&z1, sizeof(double));
	fp.write ((char*)&x2, sizeof(double));
	fp.write ((char*)&y2, sizeof(double));
	fp.write ((char*)&z2, sizeof(double));

	// write my children
	if (!this->isLeaf())
	{
		child1->saveToFile(fp);
		child2->saveToFile(fp);
	}
}


void BoundingBox::saveAllToFile(std::string dir, int id)
{
	// .bbb
	std::stringstream sstream;
	sstream << dir.c_str() << "/" << id << ".bbb";

	// output
	ofstream fp (sstream.str().c_str(), ios::out | ios::binary);

	this->saveToFile(fp);

	fp.flush();
	fp.close();
	fp.clear();
}