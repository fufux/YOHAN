#include "Face.h"

Face::Face(void)
{
}

Face::Face(vector<Vector3d*>* _vertices, int _owner)
{
	vertices = _vertices;
	owner = _owner;
}

Face::Face(vector<Point*>* pts, int i1, int i2, int i3, Vector3d*** m, int _owner)
{
	Point* p1 = (*pts)[i1];
	Point* p2 = (*pts)[i2];
	Point* p3 = (*pts)[i3];
	Vector3d* pI;
	// true for positive, false for negative
	bool sign;
	vertices = new vector<Vector3d*>();
	// First point of the face
	if(p1->getX()[1]<0){
		pI = new Vector3d(p1->getX()[0],p1->getX()[1],p1->getX()[2]);
		vertices->push_back(pI);
		sign = false;
	}else{
		sign = true;
	}
	// Second point of the face
	if(p2->getX()[1]<0 && sign){
		// there is a y=0 point, we add the point between p2 and p3
		vertices->push_back(m[i1][i2]);
		pI = new Vector3d(p2->getX()[0],p2->getX()[1],p2->getX()[2]);
		vertices->push_back(pI);
		sign = false;
	}else if(p2->getX()[1]<0 && !sign){
		pI = new Vector3d(p2->getX()[0],p2->getX()[1],p2->getX()[2]);
		vertices->push_back(pI);
		sign = false;
	}else if(p2->getX()[1]>=0 && !sign){
		// there is a y=0 point, we add the point between p2 and p3
		vertices->push_back(m[i1][i2]);
		sign = true;
	}else if(p2->getX()[1]>=0 && sign){
		// no point to add
		sign = true;
	}
	// Third point of the face
	if(p3->getX()[1]<0 && sign){
		// there is a y=0 point, we add the point between p3 and p1
		vertices->push_back(m[i2][i3]);
		pI = new Vector3d(p3->getX()[0],p3->getX()[1],p3->getX()[2]);
		vertices->push_back(pI);
		sign = false;
	}else if(p3->getX()[1]<0 && !sign){
		pI = new Vector3d(p3->getX()[0],p3->getX()[1],p3->getX()[2]);
		vertices->push_back(pI);
		sign = false;
	}else if(p3->getX()[1]>=0 && !sign){
		// there is a y=0 point, we add the point between p3 and p1
		vertices->push_back(m[i2][i3]);
		sign = true;
	}else if(p3->getX()[1]>=0 && sign){
		// no point to add
		sign = true;
	}
	// Finish the loop
	if((p1->getX()[1]<0 && sign) || (p1->getX()[1]>=0 && !sign)){
		// there is a y=0 point, we add the point between p1 and p2
		vertices->push_back(m[i3][i1]);
	}
	owner = _owner;
}

Face::~Face(void)
{
}

vector<Vector3d*>* Face::getVertices()
{
	return vertices;
}

int Face::getOwner()
{
	return owner;
}

int Face::size()
{
	return vertices->size();
}

Vector3d* Face::operator[] (int i)
{
	return (*vertices)[i];
}