#include "CollisionFace.h"

CollisionFace::CollisionFace(void)
{
}

CollisionFace::CollisionFace(std::vector<CollisionPoint>* _vertices, int _owner)
{
	vertices = _vertices;
	owner = _owner;
}

CollisionFace::~CollisionFace(void)
{
}

int CollisionFace::size()
{
	return this->vertices->size();
}

int CollisionFace::getOwner(){
	return this->owner;
}

CollisionPoint& CollisionFace::operator[] (int i)
{
	return (*vertices)[i];
}